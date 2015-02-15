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

#include "dcres.h"
#include "header.h"

#include "../util/alloc.h"

// Disconnect Response:
//   Octet 0: Channel
//   Octet 1: Status

bool knx_append_disconnect_response(msgbuilder* mb,
                                    const knx_disconnect_response* res) {
	return
		knx_append_header(mb, KNX_DISCONNECT_RESPONSE, 2) &&
		msgbuilder_append(mb, anona(const uint8_t, res->channel, res->status), 2);
}

bool knx_parse_disconnect_response(const uint8_t* message, size_t length,
                                   knx_disconnect_response* res) {
	if (length < 2)
		return false;

	res->channel = message[0];
	res->status = message[1];

	return true;
}
