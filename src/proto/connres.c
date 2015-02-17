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

#include "connres.h"
#include "header.h"

#include "../util/alloc.h"

#include <string.h>

// Connection Response:
//   Octet 0:     Channel
//   Octet 1:     Status
//   Octet 2-9:   Host info
//   Octet 10-13: Connection response info

// Connection Response Information
//   Octet 0: Structure length
//   Octet 1-3: Unknown

bool knx_generate_connection_response(msgbuilder* mb, const knx_connection_response* res) {
	if (!knx_generate_header(mb, KNX_CONNECTION_RESPONSE, knx_connection_response_size(res)))
		return false;

	if (res->status)
		return msgbuilder_append(mb, anona(const uint8_t, res->channel, res->status), 2);
	else
		return msgbuilder_append(mb, anona(const uint8_t, res->channel, res->status), 2) &&
		       knx_generate_host_info(mb, &res->host) &&
		       msgbuilder_append_single(mb, 4) &&
		       msgbuilder_append(mb, res->extended, 3);
}

bool knx_parse_connection_response(const uint8_t* message, size_t length,
                                   knx_connection_response* res) {
	if (length < 2)
		return false;

	res->channel = message[0];
	res->status = message[1];

	if (length >= KNX_HOST_INFO_SIZE + 6) {
		if (!knx_parse_host_info(message + 2, &res->host))
			return false;

		memcpy(res->extended, message + 11, 3);
	}

	return true;
}
