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

#include "tunnel.h"

#include "util/sockutils.h"
#include "util/address.h"
#include "util/alloc.h"
#include "util/log.h"

/*
 * Helpers
 */

static inline
bool knx_tunnel_process_connection_response(
	knx_tunnel*                    tunnel,
	const knx_connection_response* response
) {
	// This message does not interest us when we're not trying to connect
	if (tunnel->state != KNX_TUNNEL_CONNECTING)
		return false;

	// Something went wrong
	if (response->status != 0) {
		tunnel->state = KNX_TUNNEL_DISCONNECTED;
	} else {
		// Everything went fine
		tunnel->state = KNX_TUNNEL_CONNECTED;
		tunnel->channel = response->channel;
	}

	if (tunnel->state_change)
		tunnel->state_change(tunnel, tunnel->state_change_data);

	return true;
}

static inline
bool knx_tunnel_process_disconnect_request(
	knx_tunnel*                   tunnel,
	const knx_disconnect_request* request
) {
	if (tunnel->state == KNX_TUNNEL_DISCONNECTED || request->channel != tunnel->channel)
		return false;

	// Request disconnect response to be transmitted
	if (tunnel->send_message) {
		knx_disconnect_response response = {
			.channel = tunnel->channel,
			.status  = 0
		};

		size_t message_size = knx_size(KNX_DISCONNECT_RESPONSE, &response);
		uint8_t message[message_size];
		knx_generate(message, KNX_DISCONNECT_RESPONSE, &response);

		tunnel->send_message(tunnel, tunnel->send_message_data, message, message_size);
	}

	// Commit state change
	tunnel->state = KNX_TUNNEL_DISCONNECTED;

	if (tunnel->state_change)
		tunnel->state_change(tunnel, tunnel->state_change_data);

	return true;
}

static inline
bool knx_tunnel_process_disconnect_response(
	knx_tunnel*                    tunnel,
	const knx_disconnect_response* response
) {
	if (tunnel->state == KNX_TUNNEL_DISCONNECTED || response->channel != tunnel->channel)
		return false;

	// Commit state change
	tunnel->state = KNX_TUNNEL_DISCONNECTED;

	if (tunnel->state_change)
		tunnel->state_change(tunnel, tunnel->state_change_data);

	return true;
}

static inline
bool knx_tunnel_process_tunnel_request(
	knx_tunnel*               tunnel,
	const knx_tunnel_request* request
) {
	if (tunnel->state != KNX_TUNNEL_CONNECTED || tunnel->channel != request->channel)
		return false;

	// TODO: Check request's sequence number

	// Request tunnel response to be transmitted
	if (tunnel->send_message) {
		knx_tunnel_response response = {
			.channel    = request->channel,
			.seq_number = request->seq_number,
			.status     = 0
		};

		size_t message_size = knx_size(KNX_TUNNEL_RESPONSE, &response);
		uint8_t message[message_size];
		knx_generate(message, KNX_TUNNEL_RESPONSE, &response);

		tunnel->send_message(tunnel, tunnel->send_message_data, message, message_size);
	}

	// Inform user
	if (tunnel->handle_cemi)
		tunnel->handle_cemi(tunnel, tunnel->handle_cemi_data, &request->data);

	return true;
}

/*
 * Exports
 */

void knx_tunnel_init(knx_tunnel* tunnel) {
	tunnel->state = KNX_TUNNEL_DISCONNECTED;
	tunnel->channel = 0;
	tunnel->seq_number = 0;

	tunnel->state_change = NULL;
	tunnel->state_change_data = NULL;

	tunnel->send_message = NULL;
	tunnel->send_message_data = NULL;

	tunnel->handle_cemi = NULL;
	tunnel->handle_cemi_data = NULL;
}

void knx_tunnel_set_send_handler(
	knx_tunnel*        tunnel,
	knx_tunnel_send_cb callback,
	void*              data
) {
	tunnel->send_message = callback;
	tunnel->send_message_data = data;
}

void knx_tunnel_set_state_change_handler(
	knx_tunnel*                tunnel,
	knx_tunnel_state_change_cb callback,
	void*                      data
) {
	tunnel->state_change = callback;
	tunnel->state_change_data = data;
}

void knx_tunnel_set_recv_handler(
	knx_tunnel*        tunnel,
	knx_tunnel_recv_cb callback,
	void*              data
) {
	tunnel->handle_cemi = callback;
	tunnel->handle_cemi_data = data;
}

void knx_tunnel_connect(knx_tunnel* tunnel) {
	// Request disconnect response to be transmitted
	if (tunnel->send_message) {
		knx_connection_request request = {
			.type         = KNX_CONNECTION_REQUEST_TUNNEL,
			.layer        = KNX_CONNECTION_LAYER_TUNNEL,
			.control_host = KNX_HOST_INFO_NAT(KNX_PROTO_UDP),
			.tunnel_host  = KNX_HOST_INFO_NAT(KNX_PROTO_UDP)
		};

		size_t message_size = knx_size(KNX_CONNECTION_REQUEST, &request);
		uint8_t message[message_size];
		knx_generate(message, KNX_CONNECTION_REQUEST, &request);

		tunnel->send_message(tunnel, tunnel->send_message_data, message, message_size);
	}

	// Commit state change
	tunnel->state = KNX_TUNNEL_CONNECTING;
	tunnel->seq_number = 0;

	if (tunnel->state_change)
		tunnel->state_change(tunnel, tunnel->state_change_data);
}

bool knx_tunnel_process(
	knx_tunnel*    tunnel,
	const uint8_t* message,
	size_t         message_size
) {
	// Once disconnected from the gateway, we chose to ignore all messages
	if (tunnel->state == KNX_TUNNEL_DISCONNECTED)
		return false;

	knx_packet packet;
	if (!knx_parse(message, message_size, &packet))
		return false;

	switch (packet.service) {
		case KNX_CONNECTION_RESPONSE:
			return knx_tunnel_process_connection_response(tunnel, &packet.payload.conn_res);

		case KNX_DISCONNECT_REQUEST:
			return knx_tunnel_process_disconnect_request(tunnel, &packet.payload.dc_req);

		case KNX_DISCONNECT_RESPONSE:
			return knx_tunnel_process_disconnect_response(tunnel, &packet.payload.dc_res);

		case KNX_TUNNEL_REQUEST:
			return knx_tunnel_process_tunnel_request(tunnel, &packet.payload.tunnel_req);

		default:
			return true;
	}
}

int16_t knx_tunnel_send(
	knx_tunnel*     tunnel,
	const knx_cemi* frame
) {
	if (!tunnel->send_message || tunnel->state != KNX_TUNNEL_CONNECTED)
		return -1;

	knx_tunnel_request request = {
		.channel    = tunnel->channel,
		.seq_number = tunnel->seq_number++,
		.data       = *frame
	};

	size_t message_size = knx_size(KNX_TUNNEL_REQUEST, &request);
	uint8_t message[message_size];
	knx_generate(message, KNX_TUNNEL_REQUEST, &request);

	tunnel->send_message(tunnel, tunnel->send_message_data, message, message_size);

	return request.seq_number;
}

void knx_tunnel_disconnect(knx_tunnel* tunnel) {
	if (tunnel->state > KNX_TUNNEL_CONNECTED)
		return;

	if (tunnel->send_message) {
		knx_disconnect_request request = {
			.channel = tunnel->channel,
			.status  = 0,
			.host    = KNX_HOST_INFO_NAT(KNX_PROTO_UDP)
		};

		size_t message_size = knx_size(KNX_DISCONNECT_REQUEST, &request);
		uint8_t message[message_size];
		knx_generate(message, KNX_DISCONNECT_REQUEST, &request);

		tunnel->send_message(tunnel, tunnel->send_message_data, message, message_size);
	}

	// Commit state change
	tunnel->state = KNX_TUNNEL_DISCONNECTING;

	if (tunnel->state_change)
		tunnel->state_change(tunnel, tunnel->state_change_data);
}
