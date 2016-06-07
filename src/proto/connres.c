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

#include "connres.h"

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

void knx_connection_response_generate(uint8_t* buffer, const knx_connection_response* res) {
	*buffer++ = res->channel;
	*buffer++ = res->status;

	if (res->status == 0) {
		knx_host_info_generate(buffer, &res->host);
		buffer += KNX_HOST_INFO_SIZE;

		*buffer++ = 4;
		memcpy(buffer, res->extended, 3);
	}
}

bool knx_connection_response_parse(
	const uint8_t*           message,
	size_t                   message_length,
	knx_connection_response* res
) {
	if (message_length < 2)
		return false;

	res->channel = message[0];
	res->status = message[1];

	if (message_length >= KNX_HOST_INFO_SIZE + 6) {
		if (!knx_host_info_parse(message + 2, message_length - 2, &res->host))
			return false;

		memcpy(res->extended, message + 11, 3);
	}

	return true;
}
