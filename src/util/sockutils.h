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

#ifndef KNXPROTO_UTIL_SOCKUTILS_H_
#define KNXPROTO_UTIL_SOCKUTILS_H_

#include "address.h"
#include "../proto/knxnetip.h"

#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>

/**
 * Create a datagram socket.
 */
int knx_dgramsock_create(const ip4addr* local, bool reuse);

/**
 * Check if data is available.
 */
bool knx_dgramsock_ready(int sock, time_t timeout_sec, long timeout_usec);

/**
 * Send a KNXnet/IP packet.
 */
bool knx_dgramsock_send(int sock, knx_service srv, const void* payload, const ip4addr* target);

/**
 * Receive a packet from a number of endpoints.
 */
ssize_t knx_dgramsock_recv_raw(int sock, void* buffer, size_t buffer_size,
                               const ip4addr* endpoints, size_t num_endpoints);

/**
 * Receive a KNXnet/IP packet.
 */
bool knx_dgramsock_recv(int sock, uint8_t* buffer, size_t size,
                        knx_packet* packet,
                        const ip4addr* endpoints, size_t num_endpoints);

/**
 * Peek for a KNXnet/IP packet and return it's length.
 */
ssize_t knx_dgramsock_peek_knx(int sock);

/**
 * Make the file descriptor non-blocking.
 */
bool knx_socket_make_nonblocking(int fd);

/**
 * Make the file descriptor blocking.
 */
bool knx_socket_make_blocking(int fd);

/**
 * Check if the file descriptor is non-blocking.
 */
bool knx_socket_is_nonblocking(int fd);


#endif
