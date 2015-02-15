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

#include "tunnelres.h"
#include "header.h"
#include "../alloc.h"

// Tunnel Response:
//   Octet 0: Structure length
//   Octet 1: Channel
//   Octet 2: Sequence number
//   Octet 3: Status

bool knx_append_tunnel_response(msgbuilder* mb,
                                const knx_tunnel_response* res) {
	return
		knx_append_header(mb, KNX_TUNNEL_RESPONSE, 4) &&
		msgbuilder_append(mb, anona(const uint8_t, 4, res->channel, res->seq_number, res->status), 4);
}

bool knx_parse_tunnel_response(const uint8_t* message, size_t length,
                               knx_tunnel_response* res) {
	if (length < 4 || message[0] != 4)
		return false;

	res->channel = message[1];
	res->seq_number = message[2];
	res->status = message[3];

	return true;
}

