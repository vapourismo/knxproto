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

#include "connstatereq.h"
#include "header.h"
#include "../alloc.h"

// Connection State Request
//   Octet 0:   Channel
//   Octet 1:   Status
//   Octet 2-9: Host info

bool knx_append_connection_state_request(msgbuilder* mb,
                                         const knx_connection_state_request* req) {
	return
		knx_append_header(mb, KNX_CONNECTIONSTATE_REQUEST, 10) &&
		msgbuilder_append(mb, anona(const uint8_t, req->channel, req->status), 2) &&
		knx_append_host_info(mb, &req->host);
}

bool knx_parse_connection_state_request(const uint8_t* message, size_t length,
                                        knx_connection_state_request* req) {
	if (length < 10)
		return false;

	req->channel = message[0];
	req->status = message[1];

	return knx_parse_host_info(message + 2, &req->host);
}
