/* KNX Client Library
 * A library which provides the means to communicate with several
 * KNX-related devices or services.
 *
 * Copyright (C) 2014, Ole Krüger <ole@vprsm.de>
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

#include "../util/sockutils.h"
#include "../util/log.h"

// bool knx_tunnel_process_outgoing(knx_tunnel_client* conn) {
// 	knx_service service;
// 	uint8_t* buffer;
// 	ssize_t buffer_size = knx_outqueue_pop(&conn->outgoing, &buffer, &service);

// 	if (buffer_size < 0)
// 		return false;

// 	dgramsock_send(conn->sock, buffer, buffer_size, &conn->gateway);
// 	log_debug("Sent (service = 0x%04X)", service);

// 	free(buffer);

// 	return true;
// }

inline static void knx_tunnel_process_incoming(knx_tunnel_client* client) {
	if (!dgramsock_ready(client->sock, 0, 100000))
		return;

	uint8_t buffer[100];

	// FIXME: Do not allow every endpoint
	ssize_t r = dgramsock_recv(client->sock, buffer, 100, NULL, 0);
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
					pthread_cond_signal(&client->cond);
					pthread_mutex_unlock(&client->mutex);
				} else {
					log_error("Connection failed (code = %i)", pkg_in.payload.conn_res.status);

					pthread_mutex_lock(&client->mutex);
					client->state = KNX_TUNNEL_DISCONNECTED;
					pthread_cond_signal(&client->cond);
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
					pthread_cond_signal(&client->cond);
					pthread_mutex_unlock(&client->mutex);
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
				pthread_cond_signal(&client->cond);
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
				// knx_pkgqueue_enqueue(&client->incoming, &pkg_in);

				break;

			// Tunnel Response
			case KNX_TUNNEL_RESPONSE:
				if (client->state != KNX_TUNNEL_CONNECTED ||
				    client->channel != pkg_in.payload.tunnel_res.channel)
					break;

				// Signal acknowledgement
				pthread_mutex_lock(&client->mutex);
				client->ack_seq_number = pkg_in.payload.tunnel_res.seq_number;
				pthread_cond_signal(&client->cond);
				pthread_mutex_unlock(&client->mutex);

				break;

			// Everything else should be ignored
			default: break;
		}
	}
}

// void knx_tunnel_worker(knx_tunnel_client* conn) {
// 	while (conn->state != KNX_TUNNEL_DISCONNECTED) {
// 		// Check if we need to send a heartbeat
// 		if (conn->state == KNX_TUNNEL_CONNECTED &&
// 		    difftime(time(NULL), conn->last_heartbeat) >= 30) {
// 				knx_connection_state_request req = {conn->channel, 0, conn->host_info};

// 				if (dgramsock_send_knx(conn->sock, KNX_CONNECTION_STATE_REQUEST, &req, &conn->gateway))
// 					conn->last_heartbeat = time(NULL);
// 		}

// 		// Send outgoing messages
// 		while (knx_tunnel_process_outgoing(conn));

// 		// Receive one incoming message
// 		knx_tunnel_process_incoming(conn);
// 	}

// 	// Clear outgoing queue entirely
// 	while (knx_tunnel_process_outgoing(conn));
// }

// extern void* knx_tunnel_worker_thread(void* conn) {
// 	knx_tunnel_worker(conn);
// 	pthread_exit(NULL);
// }

// bool knx_tunnel_init(knx_tunnel_client* conn) {
// 	conn->state = KNX_TUNNEL_DISCONNECTED;
// 	knx_pkgqueue_init(&conn->incoming);

// 	if (pthread_mutex_init(&conn->mutex, NULL) != 0) {
// 		log_error("Failed to create state lock");
// 		goto fail_state_lock;
// 	}

// 	if (pthread_cond_init(&conn->cond, NULL) != 0) {
// 		log_error("Failed to create state signal");
// 		goto fail_state_signal;
// 	}

// 	if ((conn->sock = dgramsock_create(NULL, false)) < 0) {
// 		log_error("Failed to create socket");
// 		goto fail_sock;
// 	}

// 	if (!knx_outqueue_init(&conn->outgoing)) {
// 		log_error("Failed to create outgoing queue");
// 		goto fail_outgoing;
// 	}

// 	return true;

// 	fail_outgoing:     close(conn->sock);
// 	fail_sock:         pthread_cond_destroy(&conn->cond);
// 	fail_state_signal: pthread_mutex_destroy(&conn->mutex);
// 	fail_state_lock:   knx_pkgqueue_destroy(&conn->incoming);

// 	return false;
// }

// bool knx_tunnel_connect(knx_tunnel_client* conn, const ip4addr* gateway) {
// 	conn->gateway = *gateway;
// 	conn->state = KNX_TUNNEL_CONNECTING;
// 	conn->channel = 0;
// 	conn->seq_number = 0;
// 	conn->last_heartbeat = 0;

// 	static const knx_connection_request req = {
// 		KNX_CONNECTION_REQUEST_TUNNEL,
// 		KNX_LAYER_TUNNEL,
// 		KNX_HOST_INFO_NAT(KNX_PROTO_UDP),
// 		KNX_HOST_INFO_NAT(KNX_PROTO_UDP)
// 	};

// 	// Queue a connection request
// 	if (!knx_outqueue_push(&conn->outgoing, KNX_CONNECTION_REQUEST, &req)) {
// 		conn->state = KNX_TUNNEL_DISCONNECTED;
// 		return false;
// 	}

// 	// Start the worker thread
// 	if (pthread_create(&conn->worker_thread, NULL, &knx_tunnel_worker_thread, conn) != 0) {
// 		log_error("Failed to create worker thread");

// 		conn->state = KNX_TUNNEL_DISCONNECTED;

// 		close(conn->sock);
// 		knx_pkgqueue_destroy(&conn->incoming);
// 		knx_outqueue_destroy(&conn->outgoing);

// 		return false;
// 	}

// 	return true;
// }

bool knx_tunnel_wait_state(knx_tunnel_client* conn) {
	pthread_mutex_lock(&conn->mutex);
	while (conn->state == KNX_TUNNEL_CONNECTING)
		pthread_cond_wait(&conn->cond, &conn->mutex);
	pthread_mutex_unlock(&conn->mutex);

	return conn->state == KNX_TUNNEL_CONNECTED;
}
//
// void knx_tunnel_disconnect(knx_tunnel_client* conn) {
// 	if (conn->state == KNX_TUNNEL_CONNECTED) {
// 		// Queue a disconnect request
// 		knx_disconnect_request req = {conn->channel, 0, conn->host_info};
// 		knx_outqueue_push(&conn->outgoing, KNX_DISCONNECT_REQUEST, &req);
// 	}

// 	// Set state to signal the worker thread to terminate
// 	pthread_mutex_lock(&conn->mutex);
// 	conn->state = KNX_TUNNEL_DISCONNECTED;
// 	pthread_mutex_unlock(&conn->mutex);

// 	pthread_join(conn->worker_thread, NULL);
// }

// void knx_tunnel_destroy(knx_tunnel_client* conn) {
// 	if (conn->state != KNX_TUNNEL_DISCONNECTED)
// 		knx_tunnel_disconnect(conn);

// 	// Free buffers and queues
// 	knx_pkgqueue_destroy(&conn->incoming);
// 	knx_outqueue_destroy(&conn->outgoing);

// 	// Close socket
// 	close(conn->sock);

// 	// Destroy state protectors
// 	pthread_mutex_destroy(&conn->mutex);
// 	pthread_cond_destroy(&conn->cond);
// }

void* knx_tunnel_worker_thread(void* data) {
	knx_tunnel_client* client = data;

	while (client->state < KNX_TUNNEL_DISCONNECTED) {
		// Check if we need to send a heartbeat
		if (client->state == KNX_TUNNEL_CONNECTED && difftime(time(NULL), client->last_heartbeat) >= 30) {
			knx_connection_state_request req = {client->channel, 0, client->host_info};

			if (dgramsock_send_knx(client->sock, KNX_CONNECTION_STATE_REQUEST, &req, &client->gateway))
				client->last_heartbeat = time(NULL);
		}

		knx_tunnel_process_incoming(client);
	}

	pthread_exit(NULL);
}

bool knx_tunnel_init_thread_coms(knx_tunnel_client* client) {
	if (pthread_mutex_init(&client->mutex, NULL) != 0)
		return false;

	if (pthread_mutex_init(&client->send_mutex, NULL) != 0) {
		pthread_mutex_destroy(&client->mutex);
		return false;
	}

	if (pthread_cond_init(&client->cond, NULL) != 0) {
		pthread_mutex_destroy(&client->mutex);
		pthread_mutex_destroy(&client->send_mutex);
		return false;
	}

	if (pthread_create(&client->worker, NULL, &knx_tunnel_worker_thread, client) != 0) {
		pthread_mutex_destroy(&client->mutex);
		pthread_mutex_destroy(&client->send_mutex);
		pthread_cond_destroy(&client->cond);

		return false;
	}

	return true;
}

bool knx_tunnel_connect(knx_tunnel_client* client, int sock, const ip4addr* gateway) {
	if (sock < 0 || !gateway)
		return false;

	client->sock = sock;
	client->gateway = *gateway;
	client->state = KNX_TUNNEL_CONNECTING;
	client->seq_number = 0;
	client->ack_seq_number = UINT8_MAX;

	knx_connection_request req = {
		KNX_CONNECTION_REQUEST_TUNNEL,
		KNX_LAYER_TUNNEL,
		KNX_HOST_INFO_NAT(KNX_PROTO_UDP),
		KNX_HOST_INFO_NAT(KNX_PROTO_UDP)
	};

	// Initiate connection
	if (!dgramsock_send_knx(sock, KNX_CONNECTION_REQUEST, &req, gateway)) {
		log_error("Failed to send connection request");
		return false;
	}

	// Initialise thread components
	if (!knx_tunnel_init_thread_coms(client)) {
		log_error("Failed to initialise thread components");

		client->state = KNX_TUNNEL_DISCONNECTED;
		return false;
	}

	return true;
}

void knx_tunnel_disconnect(knx_tunnel_client* client) {
	if (client->state == KNX_TUNNEL_DISCONNECTED)
		return;

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
	pthread_cond_signal(&client->cond);
	pthread_mutex_unlock(&client->mutex);
}

bool knx_tunnel_send(knx_tunnel_client* client, const void* payload, size_t length) {
	if (client->state == KNX_TUNNEL_DISCONNECTED || !knx_tunnel_wait_state(client))
		return false;

	pthread_mutex_lock(&client->send_mutex);

	knx_tunnel_request req = {client->channel, client->seq_number++, length, payload};

	if (!dgramsock_send_knx(client->sock, KNX_TUNNEL_REQUEST, &req, &client->gateway)) {
		client->seq_number = req.seq_number;
		pthread_mutex_unlock(&client->send_mutex);
		return false;
	}

	pthread_mutex_lock(&client->mutex);
	while (client->state == KNX_TUNNEL_CONNECTED && client->ack_seq_number != req.seq_number)
		pthread_cond_wait(&client->cond, &client->mutex);

	bool r = client->ack_seq_number == req.seq_number;

	pthread_mutex_unlock(&client->mutex);
	pthread_mutex_unlock(&client->send_mutex);

	return r;
}
