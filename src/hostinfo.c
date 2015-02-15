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

#include "hostinfo.h"
#include "alloc.h"

#include <string.h>

// Host Information
//   Octet 0:   Structure length
//   Octet 1:   Protocol (e.g. UDP)
//   Octet 2-5: IPv4 address
//   Octet 6-7: Port number

bool knx_append_host_info(msgbuilder* mb, const knx_host_info* host) {
	return
		msgbuilder_append(mb, anona(const uint8_t, 8, host->protocol), 2) &&
		msgbuilder_append(mb, (const uint8_t*) &host->address, 4) &&
		msgbuilder_append(mb, (const uint8_t*) &host->port, 2);
}

bool knx_parse_host_info(const uint8_t* message, knx_host_info* host) {
	if (message[0] != 8)
		return false;

	switch (message[1]) {
		case KNX_PROTO_UDP:
			host->protocol = KNX_PROTO_UDP;
			break;

		case KNX_PROTO_TCP:
			host->protocol = KNX_PROTO_TCP;
			break;

		default:
			return false;
	}

	memcpy(&host->address, message + 2, 4);
	memcpy(&host->port, message + 6, 2);

	return true;
}
