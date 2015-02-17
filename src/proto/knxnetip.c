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

#include "knxnetip.h"

#include <string.h>

bool knx_parse(const uint8_t* message, size_t length,
               knx_packet* packet) {
	if (length < 6 || message[0] != 6 || message[1] != 16)
		return false;

	uint16_t claimed_len = (message[4] << 8) | message[5];
	if (claimed_len > length || claimed_len < 6)
		return false;

	uint16_t payload_length = claimed_len - 6;
	const uint8_t* payload_ptr = message + 6;

	packet->service = (message[2] << 8) | message[3];
	switch (packet->service) {
		case KNX_CONNECTION_REQUEST:
			packet->service = KNX_CONNECTION_REQUEST;
			return knx_parse_connection_request(payload_ptr, payload_length,
			                                    &packet->payload.conn_req);

		case KNX_CONNECTION_RESPONSE:
			packet->service = KNX_CONNECTION_RESPONSE;
			return knx_parse_connection_response(payload_ptr, payload_length,
			                                     &packet->payload.conn_res);

		case KNX_CONNECTIONSTATE_REQUEST:
			packet->service = KNX_CONNECTIONSTATE_REQUEST;
			return knx_parse_connection_state_request(payload_ptr, payload_length,
			                                          &packet->payload.conn_state_req);

		case KNX_CONNECTIONSTATE_RESPONSE:
			packet->service = KNX_CONNECTIONSTATE_RESPONSE;
			return knx_parse_connection_state_response(payload_ptr, payload_length,
			                                           &packet->payload.conn_state_res);

		case KNX_DISCONNECT_REQUEST:
			packet->service = KNX_DISCONNECT_REQUEST;
			return knx_parse_disconnect_request(payload_ptr, payload_length,
			                                    &packet->payload.dc_req);

		case KNX_DISCONNECT_RESPONSE:
			packet->service = KNX_DISCONNECT_RESPONSE;
			return knx_parse_disconnect_response(payload_ptr, payload_length,
			                                     &packet->payload.dc_res);

		case KNX_TUNNEL_REQUEST:
			packet->service = KNX_TUNNEL_REQUEST;
			return knx_parse_tunnel_request(payload_ptr, payload_length,
			                                &packet->payload.tunnel_req);

		case KNX_TUNNEL_RESPONSE:
			packet->service = KNX_TUNNEL_RESPONSE;
			return knx_parse_tunnel_response(payload_ptr, payload_length,
			                                 &packet->payload.tunnel_res);

		case KNX_ROUTING_INDICATION:
			packet->service = KNX_ROUTING_INDICATION;
			return knx_parse_routing_indication(payload_ptr, payload_length,
			                                    &packet->payload.routing_ind);

		default:
			return false;
	}
}

bool knx_generate(msgbuilder* mb, knx_service service, const void* payload) {
	mb->used = 0;

	switch (service) {
		case KNX_CONNECTION_REQUEST:
			return knx_append_connection_request(mb, payload);

		case KNX_CONNECTION_RESPONSE:
			return knx_append_connection_response(mb, payload);

		case KNX_CONNECTIONSTATE_REQUEST:
			return knx_append_connection_state_request(mb, payload);

		case KNX_CONNECTIONSTATE_RESPONSE:
			return knx_append_connection_state_response(mb, payload);

		case KNX_DISCONNECT_REQUEST:
			return knx_append_disconnect_request(mb, payload);

		case KNX_DISCONNECT_RESPONSE:
			return knx_append_disconnect_response(mb, payload);

		case KNX_TUNNEL_REQUEST:
			return knx_append_tunnel_request(mb, payload);

		case KNX_TUNNEL_RESPONSE:
			return knx_append_tunnel_response(mb, payload);

		case KNX_ROUTING_INDICATION:
			return knx_append_routing_indication(mb, payload);

		default:
			return false;
	}
}
