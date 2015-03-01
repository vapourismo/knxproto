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

#include "address.h"
#include "alloc.h"

#include <netdb.h>

char* ip4addr_to_string(const ip4addr* addr) {
	char* string = newa(char, INET_ADDRSTRLEN + 7);

	if (string && inet_ntop(AF_INET, &addr->sin_addr.s_addr, string, INET_ADDRSTRLEN)) {
		size_t length = strlen(string);
		size_t rest_length = INET_ADDRSTRLEN + 7 - length;
		snprintf(string + length, rest_length, ":%i", ntohs(addr->sin_port));
	}

	return string;
}

bool ip4addr_resolve(ip4addr* addr, const char* hostname, in_port_t port) {
	struct addrinfo hints = {
		.ai_flags = 0,
		.ai_family = AF_INET,
		.ai_socktype = SOCK_DGRAM,
		.ai_protocol = 0,
		.ai_addrlen = sizeof(ip4addr),
		.ai_addr = NULL,
		.ai_canonname = NULL,
		.ai_next = NULL,
	};

	struct addrinfo* array = NULL;

	if (getaddrinfo(hostname, NULL, &hints, &array) != 0)
		return false;

	struct addrinfo* result = array;
	while (result != NULL && result->ai_family != AF_INET)
		result = result->ai_next;

	if (result) {
		memcpy(addr, result->ai_addr, result->ai_addrlen);
		addr->sin_port = htons(port);

		freeaddrinfo(array);
		return true;
	} else {
		freeaddrinfo(array);
		return false;
	}
}
