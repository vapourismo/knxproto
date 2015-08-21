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
#include "util/address.h"
#include "util/alloc.h"
#include "util/log.h"

static
void knx_tunnel_set_state(knx_tunnel_client* client, knx_tunnel_state state) {
	client->state = state;

	if (client->state_cb) {
		client->state_cb(client, state, client->state_data);
	}
}

void knx_tunnel_init(knx_tunnel_client* client, int sock,
                     knx_tunnel_state_cb on_state, void* state_data,
                     knx_tunnel_recv_cb on_recv, void* recv_data) {
	client->sock = sock;
	client->state = KNX_TUNNEL_DISCONNECTED;

	client->seq_number = 0;
	client->channel = UINT8_MAX;

	// Callback information
	client->recv_cb = on_recv;
	client->recv_data = recv_data;
	client->state_cb = on_state;
	client->state_data = state_data;
}

bool knx_tunnel_connect(knx_tunnel_client* client, const char* hostname, in_port_t port) {
	if (!ip4addr_resolve(&client->gateway, hostname, port)) {
		knx_log_error("Failed to resolve %s:%i", hostname, port);
		return false;
	}

	knx_connection_request req = {
		KNX_CONNECTION_REQUEST_TUNNEL,
		KNX_CONNECTION_LAYER_TUNNEL,
		KNX_HOST_INFO_NAT(KNX_PROTO_UDP),
		KNX_HOST_INFO_NAT(KNX_PROTO_UDP)
	};

	if (knx_dgramsock_send(client->sock, KNX_CONNECTION_REQUEST, &req, &client->gateway)) {
		knx_tunnel_set_state(client, KNX_TUNNEL_CONNECTING);
		return true;
	} else {
		knx_log_error("Failed to send connection request");
		return false;
	}
}

bool knx_tunnel_disconnect(knx_tunnel_client* client) {
	if (client->state == KNX_TUNNEL_DISCONNECTED)
		return true;

	// Set new state
	knx_tunnel_set_state(client, KNX_TUNNEL_DISCONNECTED);

	knx_disconnect_request dc_req = {
		client->channel,
		0,
		client->host_info
	};

	// Send disconnect request
	if (!knx_dgramsock_send(client->sock, KNX_DISCONNECT_REQUEST, &dc_req, &client->gateway)) {
		knx_log_error("Failed to send disconnect request");
		return false;
	} else {
		return true;
	}
}

bool knx_tunnel_send(knx_tunnel_client* client, const knx_ldata* ldata) {
	if (client->state == KNX_TUNNEL_DISCONNECTED)
		return false;

	knx_tunnel_request req = {
		client->channel,
		client->seq_number++,
		{
			KNX_CEMI_LDATA_REQ,
			0,
			NULL,
			{.ldata = *ldata}
		}
	};

	return knx_dgramsock_send(client->sock, KNX_TUNNEL_REQUEST, &req, &client->gateway);
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

bool knx_tunnel_send_heartbeat(knx_tunnel_client* client) {
	knx_connection_state_request req = {client->channel, 0, client->host_info};
	return knx_dgramsock_send(client->sock, KNX_CONNECTION_STATE_REQUEST, &req, &client->gateway);
}

bool knx_tunnel_process(knx_tunnel_client* client) {
	ssize_t buffer_size = knx_dgramsock_peek_knx(client->sock);
	if (buffer_size == 0) {
		// This is not a KNXnet/IP packet so we'll discard it
		recvfrom(client->sock, NULL, 0, 0, NULL, NULL);
		return false;
	} else if (buffer_size < 0) {
		return false;
	}

	uint8_t buffer[buffer_size];
	ssize_t buffer_rv = knx_dgramsock_recv_raw(client->sock, buffer, buffer_size,
	                                           &client->gateway, 1);

	// KNXnet header is mandatory
	if (buffer_rv < KNX_HEADER_SIZE)
		return false;

	knx_packet pkg_in;

	// Parse and process the packet
	if (knx_parse(buffer, buffer_rv, &pkg_in))
		return knx_tunnel_process_packet(client, &pkg_in);
	else
		return false;
}

bool knx_tunnel_process_packet(knx_tunnel_client* client, const knx_packet* pkg_in) {
	knx_log_debug("Received (service = 0x%04X)", pkg_in->service);

	switch (pkg_in->service) {
		// Result of a connection request (duh)
		case KNX_CONNECTION_RESPONSE:
			if (client->state != KNX_TUNNEL_CONNECTING)
				return false;

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
				return false;

			knx_log_info("Heartbeat (status = %i)", pkg_in->payload.conn_state_res.status);

			// Anything other than 0 means the bad news
			if (pkg_in->payload.conn_state_res.status != 0)
				knx_tunnel_set_state(client, KNX_TUNNEL_DISCONNECTED);

			break;

		// Received when the gateway terminates
		case KNX_DISCONNECT_REQUEST:
			if (pkg_in->payload.dc_req.channel != client->channel)
				return false;

			// If connection was previously intact
			if (client->state != KNX_TUNNEL_DISCONNECTED) {
				knx_log_info("Disconnected (channel = %i, status = %i)",
				             pkg_in->payload.dc_req.channel,
				             pkg_in->payload.dc_req.status);

				knx_tunnel_set_state(client, KNX_TUNNEL_DISCONNECTED);
			}


		// Result of a disconnect request (duh)
		case KNX_DISCONNECT_RESPONSE:
			if (pkg_in->payload.dc_res.channel != client->channel)
				return false;

			// If connection was previously intact
			if (client->state != KNX_TUNNEL_DISCONNECTED) {
				knx_log_info("Disconnected (channel = %i, status = %i)",
				             pkg_in->payload.dc_res.channel,
				             pkg_in->payload.dc_res.status);

				knx_tunnel_set_state(client, KNX_TUNNEL_DISCONNECTED);
			}

			break;

		// Tunnel Request
		case KNX_TUNNEL_REQUEST:
			if (client->state != KNX_TUNNEL_CONNECTED ||
			    client->channel != pkg_in->payload.tunnel_req.channel)
				return false;

			knx_tunnel_response res = {
				client->channel,
				pkg_in->payload.tunnel_req.seq_number,
				0
			};

			// Send a tunnel response
			knx_dgramsock_send(client->sock, KNX_TUNNEL_RESPONSE, &res, &client->gateway);

			switch (pkg_in->payload.tunnel_req.data.service) {
				case KNX_CEMI_LDATA_REQ:
				case KNX_CEMI_LDATA_IND:
				case KNX_CEMI_LDATA_CON:
					if (client->recv_cb)
						client->recv_cb(client, &pkg_in->payload.tunnel_req.data.payload.ldata,
						                client->recv_data);
					break;

				default:
					knx_log_error("Unsupported CEMI service %02X",
					              pkg_in->payload.tunnel_req.data.service);
					return false;
			}

			break;

		// Tunnel Response
		case KNX_TUNNEL_RESPONSE:
			if (client->state != KNX_TUNNEL_CONNECTED ||
			    client->channel != pkg_in->payload.tunnel_res.channel)
				break;

			// ...

			break;

		// Everything else should be ignored
		default:
			knx_log_warn("Unsupported KNXnet/IP service 0x%04X", pkg_in->service);
			return false;
	}

	return true;
}

// static
// void knx_tunnel_worker_cb_read(struct ev_loop* loop, struct ev_io* watcher, int revents) {
// 	knx_tunnel_client* client = watcher->data;
// 	knx_tunnel_process(client);
// }

// static
// void knx_tunnel_worker_cb_heartbeat(struct ev_loop* loop, struct ev_timer* watcher, int revents) {
// 	knx_tunnel_client* client = watcher->data;
// 	knx_tunnel_send_heartbeat(client);
// }

// void knx_tunnel_start(knx_tunnel_client* client, struct ev_loop* loop) {
// 	knx_socket_make_nonblocking(client->sock);

// 	// Packet processor
// 	ev_io_init(&client->ev_read, knx_tunnel_worker_cb_read, client->sock, EV_READ);
// 	client->ev_read.data = client;
// 	ev_io_start(loop, &client->ev_read);

// 	// Heartbeat timer
// 	ev_timer_init(&client->ev_heartbeat, knx_tunnel_worker_cb_heartbeat, 25, 25);
// 	client->ev_heartbeat.data = client;
// 	ev_timer_start(loop, &client->ev_heartbeat);
// }

// void knx_tunnel_stop(knx_tunnel_client* client, struct ev_loop* loop) {
// 	ev_io_stop(loop, &client->ev_read);
// 	ev_timer_stop(loop, &client->ev_heartbeat);
// }
