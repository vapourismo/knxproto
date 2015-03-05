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

#ifndef KNXCLIENT_UTIL_DGRAMSOCK
#define KNXCLIENT_UTIL_DGRAMSOCK

#include "address.h"
#include "../proto/knxnetip.h"

#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>

/**
 * Create a datagram socket.
 */
int dgramsock_create(const ip4addr* local, bool reuse);

/**
 * Check if data is available.
 */
bool dgramsock_ready(int sock, time_t timeout_sec, long timeout_usec);

/**
 * Receive data from a number of given endpoints.
 */
ssize_t dgramsock_recv(int sock, void* buffer, size_t buffer_size,
                       const ip4addr* endpoints, size_t num_endpoints);

/**
 * Send a datagram.
 */
inline bool dgramsock_send(int sock, const void* buffer, size_t buffer_size,
                           const ip4addr* target) {
	ssize_t r = sendto(sock, buffer, buffer_size, 0,
	                   (const struct sockaddr*) target, sizeof(ip4addr));

	if (r < 0)
		return false;
	else
		return (size_t) r == buffer_size;
}

/**
 * Send a KNXnet/IP packet.
 */
bool dgramsock_send_knx(int sock, knx_service srv, const void* payload, const ip4addr* target);

/**
 * Receive a KNXnet/IP packet.
 */
inline bool dgramsock_recv_knx(int sock, uint8_t* buffer, size_t size,
                               knx_packet* packet, const ip4addr* endpoints,
                               size_t num_endpoints) {
	ssize_t rv = dgramsock_recv(sock, buffer, size, endpoints, num_endpoints);
	return rv > 0 && knx_parse(buffer, rv, packet);
}

/**
 * Peek for a KNXnet/IP packet and return it's length.
 */
ssize_t dgramsock_peek_knx(int sock);

#endif
