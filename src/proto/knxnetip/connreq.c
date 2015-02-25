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

#include "connreq.h"

#include "../../util/alloc.h"

// Connection Request:
//   Octet 0-5:   Header
//   Octet 6-13:  Control host information
//   Octet 14-21: Tunnelling host information
//   Octet 22-25: Connection request information

// Connection Request Information:
//   Octet 0: Structure length
//   Octet 1: Connection type
//   Octet 2: KNX Layer
//   Octet 3: Reserved (should be 0)

void knx_connection_request_generate(uint8_t* buffer, const knx_connection_request* conn_req) {
	knx_host_info_generate(buffer, &conn_req->control_host);
	buffer += KNX_HOST_INFO_SIZE;

	knx_host_info_generate(buffer, &conn_req->tunnel_host);
	buffer += KNX_HOST_INFO_SIZE;

	*buffer++ = 4;
	*buffer++ = conn_req->type;
	*buffer++ = conn_req->layer;
	*buffer++ = 0;
}

bool knx_parse_connection_request(const uint8_t* message, size_t length, knx_connection_request* req) {
	if (length < KNX_CONNECTION_REQUEST_SIZE || message[16] != 4)
		return false;

	// This seems redundant, but is required for the
	// purposes of extensibility.
	switch (message[17]) {
		case KNX_CONNECTION_REQUEST_TUNNEL:
			req->type = KNX_CONNECTION_REQUEST_TUNNEL;
			break;

		default:
			return false;
	}

	// This looks redundant aswell. Same purpose here.
	switch (message[18]) {
		case KNX_LAYER_TUNNEL:
			req->layer = KNX_LAYER_TUNNEL;
			break;

		default:
			return false;
	}

	return
		knx_parse_host_info(message, &req->control_host) &&
		knx_parse_host_info(message + 8, &req->tunnel_host);
}
