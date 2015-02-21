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

#ifndef KNXCLIENT_UTIL_ADDRESS_H
#define KNXCLIENT_UTIL_ADDRESS_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdint.h>

/**
 * IPv4 Address
 */
typedef struct sockaddr_in ip4addr;

/**
 * Generate an IPv4 address from a string and a given port.
 */
inline void ip4addr_from_string(ip4addr* addr, const char* addrstr, uint16_t port) {
	memset(addr, 0, sizeof(ip4addr));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	inet_pton(AF_INET, addrstr, &addr->sin_addr);
}

/**
 * KNX Address
 */
typedef uint16_t knx_addr;

#endif
