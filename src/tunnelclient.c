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

#include <fcntl.h>
#include <sys/time.h>

// Temporary configuration
#define KNX_TUNNEL_CONNECTION_TIMEOUT 5       // 5 Seconds
#define KNX_TUNNEL_HEARTBEAT_TIMEOUT 30       // 5 Seconds
#define KNX_TUNNEL_ACK_TIMEOUT 5              // 5 Seconds
#define KNX_TUNNEL_READ_TIMEOUT 100000        // 100ms

static void knx_tunnel_set_state(knx_tunnel_client* client, knx_tunnel_state state) {
	pthread_mutex_lock(&client->mutex);
	client->state = state;
	pthread_cond_broadcast(&client->cond);
	pthread_mutex_unlock(&client->mutex);
}

static void knx_tunnel_queue(knx_tunnel_client* client, const knx_tunnel_request* req) {
	switch (req->data.service) {
		case KNX_CEMI_LDATA_REQ:
		case KNX_CEMI_LDATA_IND:
		case KNX_CEMI_LDATA_CON: {
			knx_tunnel_message* msg = new(knx_tunnel_message);

			if (!msg)
				break;

			// Prepare element
			msg->ldata = knx_ldata_duplicate(&req->data.payload.ldata);
			msg->next = NULL;

			if (!msg->ldata) {
				free(msg);
				break;
			}

			pthread_mutex_lock(&client->mutex);

			// Insert element
			if (client->msg_head)
				client->msg_tail = client->msg_tail->next = msg;
			else
				client->msg_tail = client->msg_head = msg;

			pthread_cond_signal(&client->cond);
			pthread_mutex_unlock(&client->mutex);
			break;
		}

		default:
			knx_log_error("Unsupported CEMI service %02X", req->data.service);
			break;
	}
}

void knx_tunnel_process_packet(knx_tunnel_client* client, const knx_packet* pkg_in) {
	knx_log_debug("Received (service = 0x%04X)", pkg_in->service);

	switch (pkg_in->service) {
		// Result of a connection request (duh)
		case KNX_CONNECTION_RESPONSE:
			if (client->state != KNX_TUNNEL_CONNECTING)
				break;

			if (pkg_in->payload.conn_res.status == 0) {
				// Save channel and host info
				client->channel = pkg_in->payload.conn_res.channel;
				client->host_info = pkg_in->payload.conn_res.host;

				knx_log_info("Connected (channel = %i)", client->channel);
				knx_tunnel_set_state(client, KNX_TUNNEL_CONNECTED);
			} else {
				knx_log_error("Connection failed (code = %i)", pkg_in->payload.conn_res.status);
				knx_tunnel_set_state(client, KNX_TUNNEL_DISCONNECTED);
			}

			break;

		// Heartbeat
		case KNX_CONNECTION_STATE_RESPONSE:
			if (pkg_in->payload.conn_state_res.channel != client->channel ||
			    client->state != KNX_TUNNEL_CONNECTED)
				break;

			knx_log_info("Heartbeat (status = %i)", pkg_in->payload.conn_state_res.status);

			// Anything other than 0 means the bad news
			if (pkg_in->payload.conn_state_res.status != 0) {
				knx_tunnel_set_state(client, KNX_TUNNEL_DISCONNECTED);
			} else {
				client->heartbeat = true;
			}

			break;

		// Result of a disconnect request (duh)
		case KNX_DISCONNECT_RESPONSE:
			if (pkg_in->payload.dc_res.channel != client->channel)
				break;

			// If connection was previously intact
			if (client->state != KNX_TUNNEL_DISCONNECTED)
				knx_log_info("Disconnected (channel = %i, status = %i)",
				             pkg_in->payload.dc_req.channel,
				             pkg_in->payload.dc_req.status);

			// Entering this state will stop the worker gently
			knx_tunnel_set_state(client, KNX_TUNNEL_DISCONNECTED);

			break;

		// Tunnel Request
		case KNX_TUNNEL_REQUEST:
			if (client->state != KNX_TUNNEL_CONNECTED ||
			    client->channel != pkg_in->payload.tunnel_req.channel)
				break;

			knx_tunnel_response res = {
				client->channel,
				pkg_in->payload.tunnel_req.seq_number,
				0
			};

			// Send a tunnel response
			knx_dgramsock_send(client->sock, KNX_TUNNEL_RESPONSE, &res, &client->gateway);
			knx_tunnel_queue(client, &pkg_in->payload.tunnel_req);

			break;

		// Tunnel Response
		case KNX_TUNNEL_RESPONSE:
			if (client->state != KNX_TUNNEL_CONNECTED ||
			    client->channel != pkg_in->payload.tunnel_res.channel)
				break;

			// Signal acknowledgement
			pthread_mutex_lock(&client->mutex);
			client->ack_seq_number = pkg_in->payload.tunnel_res.seq_number;
			pthread_cond_broadcast(&client->cond);
			pthread_mutex_unlock(&client->mutex);

			break;

		// Everything else should be ignored
		default:
			knx_log_warn("Unsupported KNXnet/IP service 0x%04X", pkg_in->service);
			break;
	}
}

static void knx_tunnel_init_disconnect(knx_tunnel_client* client) {
	knx_disconnect_request dc_req = {
		client->channel,
		0,
		client->host_info
	};

	// Send disconnect request
	if (!knx_dgramsock_send(client->sock, KNX_DISCONNECT_REQUEST, &dc_req, &client->gateway))
		knx_log_error("Failed to send disconnect request");

	// Set new state
	knx_tunnel_set_state(client, KNX_TUNNEL_DISCONNECTED);
}

static void knx_tunnel_worker_cb_read(struct ev_loop* loop, struct ev_io* watcher, int revents) {
	knx_tunnel_client* client = watcher->data;

	ssize_t buffer_size = knx_dgramsock_peek_knx(client->sock);
	if (buffer_size < 0) {
		// This is not a KNXnet/IP packet so we'll discard it
		recvfrom(client->sock, NULL, 0, 0, NULL, NULL);
		return;
	}

	uint8_t buffer[buffer_size];
	ssize_t buffer_rv = knx_dgramsock_recv_raw(client->sock, buffer, buffer_size,
	                                           &client->gateway, 1);

	// KNXnet header is mandatory
	if (buffer_rv < KNX_HEADER_SIZE)
		return;

	knx_packet pkg_in;

	// Parse and process the packet
	if (knx_parse(buffer, buffer_rv, &pkg_in))
		knx_tunnel_process_packet(client, &pkg_in);
}

static void knx_tunnel_worker_cb_heartbeat(struct ev_loop* loop, struct ev_timer* watcher,
                                           int revents) {
	knx_tunnel_client* client = watcher->data;
	knx_connection_state_request req = {client->channel, 0, client->host_info};
	knx_dgramsock_send(client->sock, KNX_CONNECTION_STATE_REQUEST, &req, &client->gateway);
}

static void* knx_tunnel_worker_thread(void* data) {
	knx_tunnel_client* client = data;
	ev_loop(client->ev_loop, 0);
	pthread_exit(NULL);
}

inline static void mk_timespec(struct timespec* ts, long sec, long nsec) {
	struct timeval tv;
	gettimeofday(&tv, NULL);

	while (nsec >= 1000000000) {
		sec++;
		nsec -= 1000000000;
	}

	ts->tv_sec = tv.tv_sec + sec;
	ts->tv_nsec = tv.tv_usec * 1000 + nsec;

	while (ts->tv_nsec >= 1000000000) {
		ts->tv_sec++;
		ts->tv_nsec -= 1000000000;
	}
}

static bool knx_tunnel_timed_wait_state(knx_tunnel_client* conn, long sec, long nsec) {
	struct timespec ts;
	mk_timespec(&ts, sec, nsec);

	pthread_mutex_lock(&conn->mutex);
	while (conn->state == KNX_TUNNEL_CONNECTING &&
	       pthread_cond_timedwait(&conn->cond, &conn->mutex, &ts) == 0);

	bool r = conn->state == KNX_TUNNEL_CONNECTED;
	pthread_mutex_unlock(&conn->mutex);

	return r;
}

static bool knx_tunnel_timed_wait_ack(knx_tunnel_client* conn, uint8_t number, long sec, long nsec) {
	struct timespec ts;
	mk_timespec(&ts, sec, nsec);

	pthread_mutex_lock(&conn->mutex);
	while (conn->state == KNX_TUNNEL_CONNECTED && conn->ack_seq_number != number &&
	       pthread_cond_timedwait(&conn->cond, &conn->mutex, &ts) == 0);

	bool r = conn->ack_seq_number == number;
	pthread_mutex_unlock(&conn->mutex);

	return r;
}

static void knx_tunnel_del_events(knx_tunnel_client* client) {
	ev_io_stop(client->ev_loop, &client->ev_read);
	ev_timer_stop(client->ev_loop, &client->ev_heartbeat);
}

static void knx_tunnel_free_events(knx_tunnel_client* client) {
	if (client->ev_loop) {
		ev_loop_destroy(client->ev_loop);
		client->ev_loop = NULL;
	}
}

static bool knx_tunnel_create_events(knx_tunnel_client* client) {
	// TODO: Figure out if libev needs thread backend intialization
	// if (evthread_use_pthreads() != 0) {
	// 	knx_log_error("Failed to enable pthreads for libevent");
	// 	return false;
	// }

	client->ev_loop = ev_loop_new(0);

	if (!client->ev_loop) {
		knx_log_error("Failed to allocate event loop");
		return false;
	}

	// Packet processor
	ev_io_init(&client->ev_read, knx_tunnel_worker_cb_read, client->sock, EV_READ);
	client->ev_read.data = client;
	ev_io_start(client->ev_loop, &client->ev_read);

	// Heartbeat timer
	ev_timer_init(&client->ev_heartbeat, knx_tunnel_worker_cb_heartbeat, 30, 30);
	client->ev_heartbeat.data = client;
	ev_timer_start(client->ev_loop, &client->ev_heartbeat);

	return true;
}

static bool knx_tunnel_create_pthread(knx_tunnel_client* client) {
	if (!knx_tunnel_create_events(client))
		return false;

	if (pthread_mutex_init(&client->mutex, NULL) != 0)
		goto clean_events;

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

	clean_events:
		knx_tunnel_del_events(client);
		knx_tunnel_free_events(client);

	return false;
}

static void knx_tunnel_free_pthread(knx_tunnel_client* client) {
	knx_tunnel_del_events(client);

	pthread_join(client->worker, NULL);
	pthread_mutex_destroy(&client->mutex);
	pthread_mutex_destroy(&client->send_mutex);
	pthread_cond_destroy(&client->cond);

	knx_tunnel_free_events(client);
}

static void knx_tunnel_clear_queue(knx_tunnel_client* client) {
	knx_tunnel_message* msg = client->msg_head;

	while (msg) {
		knx_tunnel_message* free_me = msg;
		msg = msg->next;

		free(free_me->ldata);
		free(free_me);
	}

	client->msg_head = client->msg_tail = NULL;
}

static bool knx_tunnel_send_raw(knx_tunnel_client* client, const knx_ldata* ldata) {
	if (client->state == KNX_TUNNEL_DISCONNECTED)
		return false;

	pthread_mutex_lock(&client->send_mutex);

	// Send tunnel request
	knx_tunnel_request req = {
		client->channel,
		client->seq_number++,
		{
			KNX_CEMI_LDATA_REQ,
			0,
			NULL,
			{ .ldata = *ldata }
		}
	};

	if (!knx_dgramsock_send(client->sock, KNX_TUNNEL_REQUEST, &req, &client->gateway)) {
		client->seq_number = req.seq_number;
		pthread_mutex_unlock(&client->send_mutex);
		return false;
	}

	// Wait for tunnel response
	bool r = knx_tunnel_timed_wait_ack(client, req.seq_number, KNX_TUNNEL_ACK_TIMEOUT, 0);
	pthread_mutex_unlock(&client->send_mutex);

	return r;
}

static bool knx_tunnel_request_connection(knx_tunnel_client* client) {
	knx_connection_request req = {
		KNX_CONNECTION_REQUEST_TUNNEL,
		KNX_LAYER_TUNNEL,
		KNX_HOST_INFO_NAT(KNX_PROTO_UDP),
		KNX_HOST_INFO_NAT(KNX_PROTO_UDP)
	};

	return knx_dgramsock_send(client->sock, KNX_CONNECTION_REQUEST, &req, &client->gateway);
}

bool knx_tunnel_connect(knx_tunnel_client* client, const char* hostname, in_port_t port) {
	if (!ip4addr_resolve(&client->gateway, hostname, port)) {
		knx_log_error("Failed to resolve hostname '%s'", hostname);
		return false;
	}

	if ((client->sock = knx_dgramsock_create(NULL, false)) < 0) {
		knx_log_error("Failed to create socket");
		return false;
	}

	// Initialise structure
	client->state = KNX_TUNNEL_CONNECTING;
	client->seq_number = 0;
	client->ack_seq_number = UINT8_MAX;
	client->heartbeat = false;
	client->msg_head = client->msg_tail = NULL;

	// Create thread
	if (fcntl(client->sock, F_SETFL, fcntl(client->sock, F_GETFL, 0) | O_NONBLOCK) != 0 ||
	    !knx_tunnel_create_pthread(client)) {

		knx_log_error("Failed to start thread facilities");
		goto clean_socket;
	}

	// Send connection request
	if (!knx_tunnel_request_connection(client)) {
		knx_log_error("Failed to request a connection");
		goto clean_pthread;
	}

	// Wait for connecting state to transition to connected state
	if (knx_tunnel_timed_wait_state(client, KNX_TUNNEL_CONNECTION_TIMEOUT, 0))
		return true;

	clean_pthread:
		knx_tunnel_free_pthread(client);

	clean_socket:
		close(client->sock);
		client->sock = -1;

	return false;
}

void knx_tunnel_disconnect(knx_tunnel_client* client) {
	if (client->state != KNX_TUNNEL_DISCONNECTED)
		knx_tunnel_init_disconnect(client);

	// Clean thread facilities
	knx_tunnel_free_pthread(client);

	// Close socket
	if (client->sock != -1)
		close(client->sock);

	client->sock = -1;

	// Clear incoming queue
	knx_tunnel_clear_queue(client);
}

bool knx_tunnel_send(knx_tunnel_client* client, const knx_ldata* ldata) {
	return knx_tunnel_send_raw(client, ldata);
}

bool knx_tunnel_write_group(knx_tunnel_client* client, knx_addr dest,
                            knx_dpt type, const void* value) {
	uint8_t buffer[knx_dpt_size(type)];
	knx_dpt_to_apdu(buffer, type, value);

	knx_ldata frame = {
		.control1 = {KNX_LDATA_PRIO_LOW, true, true, true, false},
		.control2 = {KNX_LDATA_ADDR_GROUP, 7},
		.source = 0,
		.destination = dest,
		.tpdu = {
			.tpci = KNX_TPCI_UNNUMBERED_DATA,
			.info = {
				.data = {
					.apci = KNX_APCI_GROUPVALUEWRITE,
					.payload = buffer,
					.length = sizeof(buffer)
				}
			}
		}
	};

	return knx_tunnel_send(client, &frame);
}

knx_ldata* knx_tunnel_recv(knx_tunnel_client* client, bool block) {
	if (client->state == KNX_TUNNEL_DISCONNECTED ||
	    (!block && client->msg_head == NULL))
		return NULL;

	pthread_mutex_lock(&client->mutex);

	while (client->state == KNX_TUNNEL_CONNECTED && client->msg_head == NULL)
		pthread_cond_wait(&client->cond, &client->mutex);

	knx_ldata* data = NULL;

	if (client->msg_head != NULL) {
		knx_tunnel_message* head = client->msg_head;

		data = head->ldata;
		client->msg_head = head->next;

		free(head);
		pthread_cond_signal(&client->cond);
	}

	pthread_mutex_unlock(&client->mutex);
	return data;
}
