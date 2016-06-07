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

#include "tunnelreq.h"

#include "../util/alloc.h"

#include <string.h>

// Tunnel Request:
//   Octet 0:   Structure length
//   Octet 1:   Channel
//   Octet 2:   Sequence number
//   Octet 3:   Reserved
//   Octet 4-n: Payload

void knx_tunnel_request_generate(uint8_t* buffer, const knx_tunnel_request* req) {
	*buffer++ = 4;
	*buffer++ = req->channel;
	*buffer++ = req->seq_number;
	*buffer++ = 0;

	knx_cemi_generate(buffer, &req->data);
}

bool knx_tunnel_request_parse(
	const uint8_t*      message,
	size_t              message_length,
	knx_tunnel_request* req
) {
	if (message_length < 4 || message[0] != 4)
		return false;

	req->channel = message[1];
	req->seq_number = message[2];

	return knx_cemi_parse(message + 4, message_length - 4, &req->data);
}
