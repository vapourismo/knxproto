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

#include "tunnelreq.h"
#include "header.h"

#include "../util/alloc.h"

#include <string.h>

// Tunnel Request:
//   Octet 0:   Structure length
//   Octet 1:   Channel
//   Octet 2:   Sequence number
//   Octet 3:   Reserved
//   Octet 4-n: Payload

void knx_generate_tunnel_request(uint8_t* buffer, const knx_tunnel_request* req) {
	*buffer++ = 4;
	*buffer++ = req->channel;
	*buffer++ = req->seq_number;
	*buffer++ = 0;

	memcpy(buffer, req->data, req->size);
}

bool knx_parse_tunnel_request(const uint8_t* message, size_t length, knx_tunnel_request* req) {
	if (length < 4 || message[0] != 4)
		return false;

	req->channel = message[1];
	req->seq_number = message[2];
	req->size = length - 4;
	req->data = message + 4;

	return true;
}
