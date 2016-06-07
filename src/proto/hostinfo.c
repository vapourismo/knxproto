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

#include "hostinfo.h"

#include "../util/alloc.h"

#include <string.h>

// Host Information
//   Octet 0:   Structure length
//   Octet 1:   Protocol (e.g. UDP)
//   Octet 2-5: IPv4 address
//   Octet 6-7: Port number

void knx_host_info_generate(uint8_t* buffer, const knx_host_info* host) {
	buffer[0] = KNX_HOST_INFO_SIZE;
	buffer[1] = host->protocol;

	memcpy(buffer + 2, &host->address, 4);
	memcpy(buffer + 6, &host->port, 2);
}

bool knx_host_info_parse(const uint8_t* message, size_t message_length, knx_host_info* host) {
	if (message_length < KNX_HOST_INFO_SIZE || message[0] != KNX_HOST_INFO_SIZE)
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
