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

#include "proto.h"

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

ssize_t knx_unpack_header(
	const uint8_t* buffer,
	size_t         buffer_length,
	knx_service*   service
) {
	if (buffer == NULL || buffer_length < KNX_HEADER_SIZE)
		return -KNX_INVALID_BUFFER;

	uint16_t packet_length = buffer[4] << 8 | buffer[5];

	// Loosely check for a valid header. One would need to verify if the given service identifier is
	// valid and that the packet length does not exceed the buffer length. We delegate this duty to
	// the users of this function.
	if (buffer[0] != KNX_HEADER_SIZE || buffer[1] != 16 || packet_length < KNX_HEADER_SIZE)
		return -KNX_INVALID_HEADER;

	if (service)
		*service = buffer[2] << 8 | buffer[3];

	return packet_length;
}

ssize_t knx_parse(
	const uint8_t* frame,
	size_t         frame_length,
	knx_packet*    output
) {
	// Unpack (and validate) header
	ssize_t unpack_result = knx_unpack_header(frame, frame_length, &output->service);
	if (unpack_result < 0)
		return unpack_result;

	// Packet length must not exceed the frame length
	if ((unsigned) unpack_result > frame_length)
		return -KNX_INVALID_BUFFER;

	// Determine payload bounds
	const uint8_t* payload = frame + KNX_HEADER_SIZE;
	size_t payload_length = unpack_result - KNX_HEADER_SIZE;

	switch (output->service) {
		case KNX_CONNECTION_REQUEST:
			return knx_connection_request_parse(
				payload,
				payload_length,
				&output->payload.conn_req
			) ? unpack_result : -KNX_INVALID_PAYLOAD;

		case KNX_CONNECTION_RESPONSE:
			return knx_connection_response_parse(
				payload,
				payload_length,
				&output->payload.conn_res
			) ? unpack_result : -KNX_INVALID_PAYLOAD;

		case KNX_CONNECTION_STATE_REQUEST:
			return knx_connection_state_request_parse(
				payload,
				payload_length,
				&output->payload.conn_state_req
			) ? unpack_result : -KNX_INVALID_PAYLOAD;

		case KNX_CONNECTION_STATE_RESPONSE:
			return knx_connection_state_response_parse(
				payload,
				payload_length,
				&output->payload.conn_state_res
			) ? unpack_result : -KNX_INVALID_PAYLOAD;

		case KNX_DISCONNECT_REQUEST:
			return knx_disconnect_request_parse(
				payload,
				payload_length,
				&output->payload.dc_req
			) ? unpack_result : -KNX_INVALID_PAYLOAD;

		case KNX_DISCONNECT_RESPONSE:
			return knx_disconnect_response_parse(
				payload,
				payload_length,
				&output->payload.dc_res
			) ? unpack_result : -KNX_INVALID_PAYLOAD;

		case KNX_TUNNEL_REQUEST:
			return knx_tunnel_request_parse(
				payload,
				payload_length,
				&output->payload.tunnel_req
			) ? unpack_result : -KNX_INVALID_PAYLOAD;

		case KNX_TUNNEL_RESPONSE:
			return knx_tunnel_response_parse(
				payload,
				payload_length,
				&output->payload.tunnel_res
			) ? unpack_result : -KNX_INVALID_PAYLOAD;

		case KNX_ROUTING_INDICATION:
			return knx_routing_indication_parse(
				payload,
				payload_length,
				&output->payload.routing_ind
			) ? unpack_result : -KNX_INVALID_PAYLOAD;

		case KNX_DESCRIPTION_REQUEST:
			return knx_description_request_parse(
				payload,
				payload_length,
				&output->payload.description_req
			) ? unpack_result : -KNX_INVALID_PAYLOAD;

		case KNX_DESCRIPTION_RESPONSE:
			return knx_description_response_parse(
				payload,
				payload_length,
				&output->payload.description_res
			) ? unpack_result : -KNX_INVALID_PAYLOAD;

		default:
			return -KNX_UNKNOWN_SERVICE;
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
