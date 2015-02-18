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

bool knx_tunnel_process_outgoing(knx_tunnel_client* conn) {
	knx_service service;
	uint8_t* buffer;
	ssize_t buffer_size = knx_outqueue_pop(&conn->outgoing, &buffer, &service);

	if (buffer_size < 0)
		return false;

	dgramsock_send(conn->sock, buffer, buffer_size, &conn->gateway);
	log_debug("Sent (service = 0x%04X)", service);

	free(buffer);

	return true;
}

void knx_tunnel_process_incoming(knx_tunnel_client* conn) {
	if (!dgramsock_ready(conn->sock, 0, 100000))
		return;

	uint8_t buffer[100];

	// FIXME: Do not allow every endpoint
	ssize_t r = dgramsock_recv(conn->sock, buffer, 100, NULL, 0);
	knx_packet pkg_in;

	if (r > 0 && knx_parse(buffer, r, &pkg_in)) {
		log_debug("Received (service = 0x%04X)", pkg_in.service);

		switch (pkg_in.service) {
			// Result of a connection request (duh)
			case KNX_CONNECTION_RESPONSE:
				if (conn->state != KNX_TUNNEL_CONNECTING)
					break;

				if (pkg_in.payload.conn_res.status == 0) {
					// Save channel and host info
					conn->channel = pkg_in.payload.conn_res.channel;
					conn->host_info = pkg_in.payload.conn_res.host;

					log_info("Connected (channel = %i)", conn->channel);

					pthread_mutex_lock(&conn->state_lock);
					conn->state = KNX_TUNNEL_CONNECTED;
					pthread_cond_signal(&conn->state_signal);
					pthread_mutex_unlock(&conn->state_lock);
				} else {
					log_error("Connection failed (code = %i)", pkg_in.payload.conn_res.status);

					pthread_mutex_lock(&conn->state_lock);
					conn->state = KNX_TUNNEL_DISCONNECTED;
					pthread_cond_signal(&conn->state_signal);
					pthread_mutex_unlock(&conn->state_lock);
				}

				break;

			// Heartbeat
			case KNX_CONNECTION_STATE_RESPONSE:
				if (pkg_in.payload.conn_state_res.channel != conn->channel)
					break;

				log_debug("Heartbeat (status = %i)", pkg_in.payload.conn_state_res.status);

				// Anything other than 0 means the bad news
				if (pkg_in.payload.conn_state_res.status != 0) {
					// TODO: Find out if we need to queue a disconnect request?

					pthread_mutex_lock(&conn->state_lock);
					conn->state = KNX_TUNNEL_DISCONNECTED;
					pthread_cond_signal(&conn->state_signal);
					pthread_mutex_unlock(&conn->state_lock);
				}

				break;

			// Result of a disconnect request (duh)
			case KNX_DISCONNECT_RESPONSE:
				if (pkg_in.payload.dc_res.channel != conn->channel)
					break;

				// If connection was previously intact
				if (conn->state != KNX_TUNNEL_DISCONNECTED)
					log_info("Disconnected (channel = %i, status = %i)",
					         pkg_in.payload.dc_req.channel,
					         pkg_in.payload.dc_req.status);

				// Entering this state will stop the worker gently
				pthread_mutex_lock(&conn->state_lock);
				conn->state = KNX_TUNNEL_DISCONNECTED;
				pthread_cond_signal(&conn->state_signal);
				pthread_mutex_unlock(&conn->state_lock);

				break;

			// Tunnel Request
			case KNX_TUNNEL_REQUEST:
				if (conn->state != KNX_TUNNEL_CONNECTED ||
				    conn->channel != pkg_in.payload.tunnel_req.channel)
					break;

				knx_tunnel_response res = {
					conn->channel,
					pkg_in.payload.tunnel_req.seq_number,
					0
				};

				// Send a tunnel response
				dgramsock_send_knx_(conn->sock, KNX_TUNNEL_RESPONSE, &res, &conn->gateway);

				// Push the message onto the incoming queue
				knx_pkgqueue_enqueue(&conn->incoming, &pkg_in);

				break;

			// Tunnel Response
			case KNX_TUNNEL_RESPONSE:
				if (conn->state != KNX_TUNNEL_CONNECTED ||
				    conn->channel != pkg_in.payload.tunnel_req.channel)
					break;

				// Push the message onto the incoming queue
				knx_pkgqueue_enqueue(&conn->incoming, &pkg_in);

				break;

			// Everything else should be ignored
			default: break;
		}
	}
}

void knx_tunnel_worker(knx_tunnel_client* conn) {
	while (conn->state != KNX_TUNNEL_DISCONNECTED) {
		// Check if we need to send a heartbeat
		if (conn->state == KNX_TUNNEL_CONNECTED &&
		    difftime(time(NULL), conn->last_heartbeat) >= 30) {
				knx_connection_state_request req = {conn->channel, 0, conn->host_info};

				if (dgramsock_send_knx_(conn->sock, KNX_CONNECTION_STATE_REQUEST, &req, &conn->gateway))
					conn->last_heartbeat = time(NULL);
		}

		// Send outgoing messages
		while (knx_tunnel_process_outgoing(conn));

		// Receive one incoming message
		knx_tunnel_process_incoming(conn);
	}

	// Clear outgoing queue entirely
	while (knx_tunnel_process_outgoing(conn));
}

extern void* knx_tunnel_worker_thread(void* conn) {
	knx_tunnel_worker(conn);
	pthread_exit(NULL);
}

bool knx_tunnel_init(knx_tunnel_client* conn) {
	conn->state = KNX_TUNNEL_DISCONNECTED;
	knx_pkgqueue_init(&conn->incoming);

	if (pthread_mutex_init(&conn->state_lock, NULL) != 0) {
		log_error("Failed to create state lock");
		goto fail_state_lock;
	}

	if (pthread_cond_init(&conn->state_signal, NULL) != 0) {
		log_error("Failed to create state signal");
		goto fail_state_signal;
	}

	if ((conn->sock = dgramsock_create(NULL, false)) < 0) {
		log_error("Failed to create socket");
		goto fail_sock;
	}

	if (!knx_outqueue_init(&conn->outgoing)) {
		log_error("Failed to create outgoing queue");
		goto fail_outgoing;
	}

	return true;

	fail_outgoing:     close(conn->sock);
	fail_sock:         pthread_cond_destroy(&conn->state_signal);
	fail_state_signal: pthread_mutex_destroy(&conn->state_lock);
	fail_state_lock:   knx_pkgqueue_destroy(&conn->incoming);

	return false;
}

bool knx_tunnel_connect(knx_tunnel_client* conn, const ip4addr* gateway) {
	conn->gateway = *gateway;
	conn->state = KNX_TUNNEL_CONNECTING;
	conn->channel = 0;
	conn->seq_number = 0;
	conn->last_heartbeat = 0;

	static const knx_connection_request req = {
		KNX_CONNECTION_REQUEST_TUNNEL,
		KNX_LAYER_TUNNEL,
		KNX_HOST_INFO_NAT(KNX_PROTO_UDP),
		KNX_HOST_INFO_NAT(KNX_PROTO_UDP)
	};

	// Queue a connection request
	if (!knx_outqueue_push(&conn->outgoing, KNX_CONNECTION_REQUEST, &req)) {
		conn->state = KNX_TUNNEL_DISCONNECTED;
		return false;
	}

	// Start the worker thread
	if (pthread_create(&conn->worker_thread, NULL, &knx_tunnel_worker_thread, conn) != 0) {
		log_error("Failed to create worker thread");

		conn->state = KNX_TUNNEL_DISCONNECTED;

		close(conn->sock);
		knx_pkgqueue_destroy(&conn->incoming);
		knx_outqueue_destroy(&conn->outgoing);

		return false;
	}

	return true;
}

bool knx_tunnel_wait_state(knx_tunnel_client* conn) {
	pthread_mutex_lock(&conn->state_lock);
	while (conn->state == KNX_TUNNEL_CONNECTING)
		pthread_cond_wait(&conn->state_signal, &conn->state_lock);
	pthread_mutex_unlock(&conn->state_lock);

	return conn->state == KNX_TUNNEL_CONNECTED;
}

// bool knx_tunnel_wait_state_timed(knx_tunnel_client* conn, const struct timespec* ts) {
// 	pthread_mutex_lock(&conn->state_lock);
// 	while (conn->state == KNX_TUNNEL_CONNECTING)
// 		pthread_cond_timedwait(&conn->state_signal, &conn->state_lock, ts)
// 	pthread_mutex_unlock(&conn->state_lock);
//
// 	return conn->state == KNX_TUNNEL_CONNECTED;
// }

void knx_tunnel_disconnect(knx_tunnel_client* conn) {
	if (conn->state == KNX_TUNNEL_CONNECTED) {
		// Queue a disconnect request
		knx_disconnect_request req = {conn->channel, 0, conn->host_info};
		knx_outqueue_push(&conn->outgoing, KNX_DISCONNECT_REQUEST, &req);
	}

	// Set state to signal the worker thread to terminate
	pthread_mutex_lock(&conn->state_lock);
	conn->state = KNX_TUNNEL_DISCONNECTED;
	pthread_mutex_unlock(&conn->state_lock);

	pthread_join(conn->worker_thread, NULL);
}

void knx_tunnel_destroy(knx_tunnel_client* conn) {
	if (conn->state != KNX_TUNNEL_DISCONNECTED)
		knx_tunnel_disconnect(conn);

	// Free buffers and queues
	knx_pkgqueue_destroy(&conn->incoming);
	knx_outqueue_destroy(&conn->outgoing);

	// Close socket
	close(conn->sock);

	// Destroy state protectors
	pthread_mutex_destroy(&conn->state_lock);
	pthread_cond_destroy(&conn->state_signal);
}

bool knx_tunnel_send(knx_tunnel_client* conn, const void* payload, size_t length) {
	if (conn->state != KNX_TUNNEL_CONNECTED)
		return false;

	knx_tunnel_request req = {conn->channel, conn->seq_number++, length, payload};
	return knx_outqueue_push(&conn->outgoing, KNX_TUNNEL_REQUEST, &req);
}
