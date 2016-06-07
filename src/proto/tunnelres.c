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

#include "tunnelres.h"

#include "../util/alloc.h"

// Tunnel Response:
//   Octet 0: Structure length
//   Octet 1: Channel
//   Octet 2: Sequence number
//   Octet 3: Status

void knx_tunnel_response_generate(uint8_t* buffer, const knx_tunnel_response* res) {
	*buffer++ = 4;
	*buffer++ = res->channel;
	*buffer++ = res->seq_number;
	*buffer++ = res->status;
}

bool knx_tunnel_response_parse(
	const uint8_t*       message,
	size_t               message_length,
	knx_tunnel_response* res
) {
	if (message_length < KNX_TUNNEL_RESPONSE_SIZE || message[0] != KNX_TUNNEL_RESPONSE_SIZE)
		return false;

	res->channel = message[1];
	res->seq_number = message[2];
	res->status = message[3];

	return true;
}

