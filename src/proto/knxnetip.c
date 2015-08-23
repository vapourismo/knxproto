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

#include "knxnetip.h"

#include <arpa/inet.h>
#include <string.h>

// Header:
//   Octet 0:   Header length
//   Octet 1:   Protocol version
//   Octet 2-3: Service number
//   Octet 4-5: Packet length including header size

bool knx_header_generate(uint8_t* buffer, knx_service srv, size_t length) {
	// Since the protocol specifies the payload length to be a 16-bit unsigned integer, we have to
	// make sure the given length + header size do not exceed the uint16_t bounds.
	if (length > UINT16_MAX - KNX_HEADER_SIZE)
		return false;

	// This preamble will always be there, unless the underlying KNXnet/IP version changes.
	buffer[0] = KNX_HEADER_SIZE;
	buffer[1] = 16;

	// Service description
	buffer[2] = srv >> 8 & 0xFF;
	buffer[3] = srv & 0xFF;

	// Entire packet size
	length += KNX_HEADER_SIZE;
	buffer[4] = length >> 8 & 0xFF;
	buffer[5] = length & 0xFF;

	return true;
}

bool knx_unpack_header(const uint8_t* buffer, knx_service* service, size_t* length) {
	if (buffer[0] != KNX_HEADER_SIZE || buffer[1] != 16)
		return false;

	if (service)
		*service = buffer[2] << 8 | buffer[3];

	if (length)
		*length = buffer[4] << 8 | buffer[5];

	return true;
}

bool knx_parse(const uint8_t* message, size_t length, knx_packet* packet) {
	if (length < KNX_HEADER_SIZE || message[0] != KNX_HEADER_SIZE || message[1] != 16)
		return false;

	uint16_t claimed_len = (message[4] << 8) | message[5];
	if (claimed_len > length || claimed_len < KNX_HEADER_SIZE)
		return false;

	uint16_t payload_length = claimed_len - KNX_HEADER_SIZE;
	const uint8_t* payload_ptr = message + KNX_HEADER_SIZE;

	packet->service = (message[2] << 8) | message[3];
	switch (packet->service) {
		case KNX_CONNECTION_REQUEST:
			return knx_connection_request_parse(payload_ptr, payload_length,
			                                    &packet->payload.conn_req);

		case KNX_CONNECTION_RESPONSE:
			return knx_connection_response_parse(payload_ptr, payload_length,
			                                     &packet->payload.conn_res);

		case KNX_CONNECTION_STATE_REQUEST:
			return knx_connection_state_request_parse(payload_ptr, payload_length,
			                                          &packet->payload.conn_state_req);

		case KNX_CONNECTION_STATE_RESPONSE:
			return knx_connection_state_response_parse(payload_ptr, payload_length,
			                                           &packet->payload.conn_state_res);

		case KNX_DISCONNECT_REQUEST:
			return knx_disconnect_request_parse(payload_ptr, payload_length,
			                                    &packet->payload.dc_req);

		case KNX_DISCONNECT_RESPONSE:
			return knx_disconnect_response_parse(payload_ptr, payload_length,
			                                     &packet->payload.dc_res);

		case KNX_TUNNEL_REQUEST:
			return knx_tunnel_request_parse(payload_ptr, payload_length,
			                                &packet->payload.tunnel_req);

		case KNX_TUNNEL_RESPONSE:
			return knx_tunnel_response_parse(payload_ptr, payload_length,
			                                 &packet->payload.tunnel_res);

		case KNX_ROUTING_INDICATION:
			return knx_routing_indication_parse(payload_ptr, payload_length,
			                                    &packet->payload.routing_ind);

		case KNX_DESCRIPTION_REQUEST:
			return knx_description_request_parse(payload_ptr, payload_length,
			                                     &packet->payload.description_req);

		default:
			return false;
	}
}

bool knx_generate(uint8_t* buffer, knx_service service, const void* payload) {
	if (!knx_header_generate(buffer, service, knx_payload_size(service, payload)))
		return false;

	switch (service) {
		case KNX_CONNECTION_REQUEST:
			knx_connection_request_generate(buffer + KNX_HEADER_SIZE, payload);
			return true;

		case KNX_CONNECTION_RESPONSE:
			knx_connection_response_generate(buffer + KNX_HEADER_SIZE, payload);
			return true;

		case KNX_CONNECTION_STATE_REQUEST:
			knx_connection_state_request_generate(buffer + KNX_HEADER_SIZE, payload);
			return true;

		case KNX_CONNECTION_STATE_RESPONSE:
			knx_connection_state_response_generate(buffer + KNX_HEADER_SIZE, payload);
			return true;

		case KNX_DISCONNECT_REQUEST:
			knx_disconnect_request_generate(buffer + KNX_HEADER_SIZE, payload);
			return true;

		case KNX_DISCONNECT_RESPONSE:
			knx_disconnect_response_generate(buffer + KNX_HEADER_SIZE, payload);
			return true;

		case KNX_TUNNEL_REQUEST:
			knx_tunnel_request_generate(buffer + KNX_HEADER_SIZE, payload);
			return true;

		case KNX_TUNNEL_RESPONSE:
			knx_tunnel_response_generate(buffer + KNX_HEADER_SIZE, payload);
			return true;

		case KNX_ROUTING_INDICATION:
			knx_routing_indication_generate(buffer + KNX_HEADER_SIZE, payload);
			return true;

		case KNX_DESCRIPTION_REQUEST:
			knx_description_request_generate(buffer + KNX_HEADER_SIZE, payload);
			return true;

		default:
			return false;
	}
}

size_t knx_payload_size(knx_service service, const void* payload) {
	switch (service) {
		case KNX_CONNECTION_REQUEST:
			return KNX_CONNECTION_REQUEST_SIZE;

		case KNX_CONNECTION_RESPONSE:
			return knx_connection_response_size(payload);

		case KNX_CONNECTION_STATE_REQUEST:
			return KNX_CONNECTION_STATE_REQUEST_SIZE;

		case KNX_CONNECTION_STATE_RESPONSE:
			return KNX_CONNECTION_STATE_RESPONSE_SIZE;

		case KNX_DISCONNECT_REQUEST:
			return KNX_DISCONNECT_REQUEST_SIZE;

		case KNX_DISCONNECT_RESPONSE:
			return KNX_DISCONNECT_RESPONSE_SIZE;

		case KNX_TUNNEL_REQUEST:
			return knx_tunnel_request_size(payload);

		case KNX_TUNNEL_RESPONSE:
			return KNX_TUNNEL_RESPONSE_SIZE;

		case KNX_ROUTING_INDICATION:
			return knx_routing_indication_size(payload);

		case KNX_DESCRIPTION_REQUEST:
			return KNX_DESCRIPTION_REQUEST_SIZE;

		default:
			return 0;
	}
}
