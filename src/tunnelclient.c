/* KNX Client Library
 * A library which provides the means to communicate with several
 * KNX-related devices or services.
 *
 * Copyright (C) 2014-2015, Ole Krüger <ole@vprsm.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "tunnelclient.h"

#include "util/sockutils.h"
#include "util/log.h"
#include "util/alloc.h"

#include <sys/time.h>

// Temporary configuration
#define KNX_TUNNEL_CONNECTION_TIMEOUT 5       // 5 Seconds
#define KNX_TUNNEL_HEARTBEAT_TIMEOUT 30        // 5 Seconds
#define KNX_TUNNEL_ACK_TIMEOUT 5              // 5 Seconds
#define KNX_TUNNEL_READ_TIMEOUT 100000        // 100ms
#define KNX_TUNNEL_QUEUE_SIZE_CAP 1073741824  // 1 MiB queue cap

inline static void knx_tunnel_process_incoming(knx_tunnel_client* client) {
	if (!dgramsock_ready(client->sock, 0, KNX_TUNNEL_READ_TIMEOUT))
		return;

	ssize_t buffer_size = dgramsock_peek_knx(client->sock);
	if (buffer_size < 0) {
		// This is not a KNXnet/IP packet so we'll discard it
		recvfrom(client->sock, NULL, 0, 0, NULL, NULL);

		return;
	}

	uint8_t buffer[buffer_size];

	ssize_t r = dgramsock_recv(client->sock, buffer, buffer_size, &client->gateway, 1);
	knx_packet pkg_in;

	if (r > 0 && knx_parse(buffer, r, &pkg_in)) {
		log_debug("Received (service = 0x%04X)", pkg_in.service);

		switch (pkg_in.service) {
			// Result of a connection request (duh)
			case KNX_CONNECTION_RESPONSE:
				if (client->state != KNX_TUNNEL_CONNECTING)
					break;

				if (pkg_in.payload.conn_res.status == 0) {
					// Save channel and host info
					client->channel = pkg_in.payload.conn_res.channel;
					client->host_info = pkg_in.payload.conn_res.host;

					log_info("Connected (channel = %i)", client->channel);

					pthread_mutex_lock(&client->mutex);
					client->state = KNX_TUNNEL_CONNECTED;
					pthread_cond_broadcast(&client->cond);
					pthread_mutex_unlock(&client->mutex);
				} else {
					log_error("Connection failed (code = %i)", pkg_in.payload.conn_res.status);

					pthread_mutex_lock(&client->mutex);
					client->state = KNX_TUNNEL_DISCONNECTED;
					pthread_cond_broadcast(&client->cond);
					pthread_mutex_unlock(&client->mutex);
				}

				break;

			// Heartbeat
			case KNX_CONNECTION_STATE_RESPONSE:
				if (pkg_in.payload.conn_state_res.channel != client->channel ||
				    client->state != KNX_TUNNEL_CONNECTED)
					break;

				log_debug("Heartbeat (status = %i)", pkg_in.payload.conn_state_res.status);

				// Anything other than 0 means the bad news
				if (pkg_in.payload.conn_state_res.status != 0) {
					pthread_mutex_lock(&client->mutex);
					client->state = KNX_TUNNEL_DISCONNECTED;
					pthread_cond_broadcast(&client->cond);
					pthread_mutex_unlock(&client->mutex);
				} else {
					client->last_heartbeat_ack = time(NULL);
				}

				break;

			// Result of a disconnect request (duh)
			case KNX_DISCONNECT_RESPONSE:
				if (pkg_in.payload.dc_res.channel != client->channel)
					break;

				// If connection was previously intact
				if (client->state != KNX_TUNNEL_DISCONNECTED)
					log_info("Disconnected (channel = %i, status = %i)",
					         pkg_in.payload.dc_req.channel,
					         pkg_in.payload.dc_req.status);

				// Entering this state will stop the worker gently
				pthread_mutex_lock(&client->mutex);
				client->state = KNX_TUNNEL_DISCONNECTED;
				pthread_cond_broadcast(&client->cond);
				pthread_mutex_unlock(&client->mutex);

				break;

			// Tunnel Request
			case KNX_TUNNEL_REQUEST:
				if (client->state != KNX_TUNNEL_CONNECTED ||
				    client->channel != pkg_in.payload.tunnel_req.channel)
					break;

				knx_tunnel_response res = {
					client->channel,
					pkg_in.payload.tunnel_req.seq_number,
					0
				};

				// Send a tunnel response
				dgramsock_send_knx(client->sock, KNX_TUNNEL_RESPONSE, &res, &client->gateway);

				// Push the message onto the incoming queue
				pthread_mutex_lock(&client->mutex);

				knx_tunnel_message* msg = NULL;
				if (client->msg_queue_size + pkg_in.payload.tunnel_req.size <= KNX_TUNNEL_QUEUE_SIZE_CAP &&
				    (msg = new(knx_tunnel_message)) &&
				    (msg->message = newa(uint8_t, pkg_in.payload.tunnel_req.size))) {

					// Prepare element
					msg->next = NULL;
					msg->size = pkg_in.payload.tunnel_req.size;
					memcpy(msg->message, pkg_in.payload.tunnel_req.data, msg->size);

					// Insert element
					if (client->msg_head)
						client->msg_tail = client->msg_tail->next = msg;
					else
						client->msg_tail = client->msg_head = msg;

					client->msg_queue_size += msg->size + sizeof(knx_tunnel_message);

					pthread_cond_signal(&client->cond);
				} else if (msg) {
					log_error("Allocating new queue element failed");
					free(msg);
				} else if (client->msg_queue_size + pkg_in.payload.tunnel_req.size > KNX_TUNNEL_QUEUE_SIZE_CAP) {
					log_info("Reached maximum queue size");
				}

				pthread_mutex_unlock(&client->mutex);

				break;

			// Tunnel Response
			case KNX_TUNNEL_RESPONSE:
				if (client->state != KNX_TUNNEL_CONNECTED ||
				    client->channel != pkg_in.payload.tunnel_res.channel)
					break;

				// Signal acknowledgement
				pthread_mutex_lock(&client->mutex);
				client->ack_seq_number = pkg_in.payload.tunnel_res.seq_number;
				pthread_cond_broadcast(&client->cond);
				pthread_mutex_unlock(&client->mutex);

				break;

			// Everything else should be ignored
			default:
				log_warn("Unsupported service 0x%04X", pkg_in.service);
				break;
		}
	}
}

void knx_tunnel_init_disconnect(knx_tunnel_client* client) {
	knx_disconnect_request dc_req = {
		client->channel,
		0,
		client->host_info
	};

	// Send disconnect request
	if (!dgramsock_send_knx(client->sock, KNX_DISCONNECT_REQUEST, &dc_req, &client->gateway))
		log_error("Failed to send disconnect request");

	// Set new state
	pthread_mutex_lock(&client->mutex);
	client->state = KNX_TUNNEL_DISCONNECTED;
	pthread_cond_broadcast(&client->cond);
	pthread_mutex_unlock(&client->mutex);
}

void* knx_tunnel_worker_thread(void* data) {
	knx_tunnel_client* client = data;

	// Process incoming while in connecting state
	while (client->state == KNX_TUNNEL_CONNECTING)
		knx_tunnel_process_incoming(client);

	while (client->state == KNX_TUNNEL_CONNECTED) {
		time_t current_time = time(NULL);
		double diff_time = difftime(current_time, client->last_heartbeat);

		// Check if last heartbeat has not been acknowledged
		if (client->last_heartbeat_ack < client->last_heartbeat && diff_time >= KNX_TUNNEL_ACK_TIMEOUT) {
			log_error("Gateway ignored heartbeat");
			knx_tunnel_init_disconnect(client);
		}

		// Check if we need to send a heartbeat
		else if (diff_time >= KNX_TUNNEL_HEARTBEAT_TIMEOUT) {
			knx_connection_state_request req = {client->channel, 0, client->host_info};

			if (dgramsock_send_knx(client->sock, KNX_CONNECTION_STATE_REQUEST, &req, &client->gateway))
				client->last_heartbeat = current_time;
		}

		knx_tunnel_process_incoming(client);
	}

	// Change state
	pthread_mutex_lock(&client->mutex);

	client->state = KNX_TUNNEL_DISCONNECTED;
	pthread_cond_broadcast(&client->cond);

	pthread_mutex_unlock(&client->mutex);
	pthread_exit(NULL);
}

bool knx_tunnel_timed_wait_state(knx_tunnel_client* conn, long sec, long nsec) {
	struct timeval tv;
	gettimeofday(&tv, NULL);

	while (nsec >= 1000000000) {
		sec++;
		nsec -= 1000000000;
	}

	struct timespec ts = {
		.tv_sec = tv.tv_sec + sec,
		.tv_nsec = tv.tv_usec * 1000 + nsec
	};

	while (ts.tv_nsec >= 1000000000) {
		ts.tv_sec++;
		ts.tv_nsec -= 1000000000;
	}

	pthread_mutex_lock(&conn->mutex);
	while (conn->state == KNX_TUNNEL_CONNECTING &&
	       pthread_cond_timedwait(&conn->cond, &conn->mutex, &ts) == 0);

	bool r = conn->state == KNX_TUNNEL_CONNECTED;
	pthread_mutex_unlock(&conn->mutex);

	return r;
}

bool knx_tunnel_timed_wait_ack(knx_tunnel_client* conn, uint8_t number, long sec, long nsec) {
	struct timeval tv;
	gettimeofday(&tv, NULL);

	while (nsec >= 1000000000) {
		sec++;
		nsec -= 1000000000;
	}

	struct timespec ts = {
		.tv_sec = tv.tv_sec + sec,
		.tv_nsec = tv.tv_usec * 1000 + nsec
	};

	while (ts.tv_nsec >= 1000000000) {
		ts.tv_sec++;
		ts.tv_nsec -= 1000000000;
	}

	pthread_mutex_lock(&conn->mutex);
	while (conn->state == KNX_TUNNEL_CONNECTED && conn->ack_seq_number != number &&
	       pthread_cond_timedwait(&conn->cond, &conn->mutex, &ts) == 0);

	bool r = conn->ack_seq_number != number;
	pthread_mutex_unlock(&conn->mutex);

	return r;
}

bool knx_tunnel_init_thread_coms(knx_tunnel_client* client) {
	if (pthread_mutex_init(&client->mutex, NULL) != 0)
		return false;

	if (pthread_mutex_init(&client->send_mutex, NULL) != 0)
		goto clean_mutex;

	if (pthread_cond_init(&client->cond, NULL) != 0)
		goto clean_send_mutex;

	if (pthread_create(&client->worker, NULL, &knx_tunnel_worker_thread, client) == 0)
		return true;

	pthread_cond_destroy(&client->cond);

	clean_send_mutex:
		pthread_mutex_destroy(&client->send_mutex);

	clean_mutex:
		pthread_mutex_destroy(&client->mutex);

	return false;
}

bool knx_tunnel_connect(knx_tunnel_client* client, const char* hostname, in_port_t port) {
	if (!ip4addr_resolve(&client->gateway, hostname, port)) {
		log_error("Failed to resolve hostname '%s'", hostname);
		return false;
	}

	if ((client->sock = dgramsock_create(NULL, false)) < 0) {
		log_error("Failed to create socket");
		return false;
	}

	client->state = KNX_TUNNEL_CONNECTING;
	client->seq_number = 0;
	client->ack_seq_number = UINT8_MAX;
	client->last_heartbeat = client->last_heartbeat_ack = time(NULL);
	client->msg_queue_size = 0;
	client->msg_head = client->msg_tail = NULL;

	knx_connection_request req = {
		KNX_CONNECTION_REQUEST_TUNNEL,
		KNX_LAYER_TUNNEL,
		KNX_HOST_INFO_NAT(KNX_PROTO_UDP),
		KNX_HOST_INFO_NAT(KNX_PROTO_UDP)
	};

	// Initiate connection
	if (!dgramsock_send_knx(client->sock, KNX_CONNECTION_REQUEST, &req, &client->gateway)) {
		log_error("Failed to send connection request");
		close(client->sock);
		return false;
	}

	// Initialise thread components
	if (!knx_tunnel_init_thread_coms(client)) {
		log_error("Failed to initialise thread components");

		client->state = KNX_TUNNEL_DISCONNECTED;
		close(client->sock);
		return false;
	}

	// Wait for connecting state to transition to connected state
	bool r = knx_tunnel_timed_wait_state(client, KNX_TUNNEL_CONNECTION_TIMEOUT, 0);

	// Connection could not be established
	if (!r) knx_tunnel_disconnect(client);

	return r;
}

void knx_tunnel_disconnect(knx_tunnel_client* client) {
	if (client->state != KNX_TUNNEL_DISCONNECTED)
		knx_tunnel_init_disconnect(client);

	// Join thread and cleanup resources
	pthread_join(client->worker, NULL);
	pthread_mutex_destroy(&client->mutex);
	pthread_mutex_destroy(&client->send_mutex);
	pthread_cond_destroy(&client->cond);

	// Close socket
	close(client->sock);
}

bool knx_tunnel_send(knx_tunnel_client* client, const void* payload, uint16_t length) {
	if (client->state == KNX_TUNNEL_DISCONNECTED)
		return false;

	pthread_mutex_lock(&client->send_mutex);

	// Send tunnel request
	knx_tunnel_request req = {client->channel, client->seq_number++, length, payload};

	if (!dgramsock_send_knx(client->sock, KNX_TUNNEL_REQUEST, &req, &client->gateway)) {
		client->seq_number = req.seq_number;
		pthread_mutex_unlock(&client->send_mutex);
		return false;
	}

	// Wait for tunnel response
	bool r = knx_tunnel_timed_wait_ack(client, req.seq_number, KNX_TUNNEL_ACK_TIMEOUT, 0);
	pthread_mutex_unlock(&client->send_mutex);
	return r;
}

bool knx_tunnel_send_ldata(knx_tunnel_client* client, const knx_ldata* ldata) {
	uint8_t buffer[knx_cemi_size(KNX_CEMI_LDATA_REQ, ldata)];
	knx_cemi_generate_(buffer, KNX_CEMI_LDATA_REQ, ldata);
	return knx_tunnel_send(client, buffer, sizeof(buffer));
}

ssize_t knx_tunnel_recv(knx_tunnel_client* client, uint8_t** buffer) {
	if (client->state == KNX_TUNNEL_DISCONNECTED)
		return -1;

	pthread_mutex_lock(&client->mutex);

	while (client->state == KNX_TUNNEL_CONNECTED && client->msg_head == NULL)
		pthread_cond_wait(&client->cond, &client->mutex);

	ssize_t size = -1;

	if (client->msg_head != NULL) {
		knx_tunnel_message* head = client->msg_head;

		size = head->size;
		if (buffer) *buffer = head->message;

		client->msg_head = head->next;
		client->msg_queue_size -= size + sizeof(knx_tunnel_message);

		free(head);
		pthread_cond_signal(&client->cond);
	}

	pthread_mutex_unlock(&client->mutex);
	return size;
}

knx_ldata* knx_tunnel_recv_ldata(knx_tunnel_client* client) {
	uint8_t* data;
	ssize_t size = knx_tunnel_recv(client, &data);

	knx_cemi_frame cemi;

	if (size < 0)
		return NULL;

	if (!knx_cemi_parse(data, size, &cemi) || cemi.service != KNX_CEMI_LDATA_IND) {
		log_error("Failed to parse as L_Data indication frame");
		free(data);
		return NULL;
	}

	// Preserve TPDU
	uint8_t tpdu[cemi.payload.ldata.length];
	memcpy(tpdu, cemi.payload.ldata.tpdu, cemi.payload.ldata.length);

	// Reallocate buffer space to fit the L_Data + TPDU
	knx_ldata* ret = realloc(data, sizeof(knx_ldata) + sizeof(tpdu));

	if (!ret)
		return NULL;

	// Copy L_Data and setup TPDU
	*ret = cemi.payload.ldata;
	ret->tpdu = (const uint8_t*) (ret + 1);
	memcpy(ret + 1, tpdu, sizeof(tpdu));

	return ret;
}

void knx_tunnel_clear_queue(knx_tunnel_client* client) {
	knx_tunnel_message* msg = client->msg_head;

	while (msg) {
		knx_tunnel_message* free_me = msg;
		msg = msg->next;

		free(free_me->message);
		free(free_me);
	}

	client->msg_head = client->msg_tail = NULL;
}
