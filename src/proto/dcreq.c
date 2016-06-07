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

#include "dcreq.h"

#include "../util/alloc.h"

// Disconnect Request:
//   Octet 0:   Channel
//   Octet 1:   Status
//   Octet 2-9: Host info

void knx_disconnect_request_generate(uint8_t* buffer, const knx_disconnect_request* req) {
	*buffer++ = req->channel;
	*buffer++ = req->status;

	knx_host_info_generate(buffer, &req->host);
}

bool knx_disconnect_request_parse(
	const uint8_t*          message,
	size_t                  message_length,
	knx_disconnect_request* req
) {
	if (message_length < KNX_DISCONNECT_REQUEST_SIZE)
		return false;

	req->channel = message[0];
	req->status = message[1];

	return knx_host_info_parse(message + 2, message_length - 2, &req->host);
}
