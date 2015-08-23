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

#include "sockutils.h"
#include "alloc.h"

#include <stdarg.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/select.h>

int knx_dgramsock_create(const ip4addr* local, bool reuse) {
	int sock;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return -1;

	if (local) {
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, anona(int, reuse), sizeof(int));

		if (bind(sock, (const struct sockaddr*) local, sizeof(ip4addr)) != 0) {
			close(sock);
			return -1;
		}
	}

	return sock;
}

bool knx_dgramsock_ready(int sock, time_t timeout_sec, long timeout_usec) {
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(sock, &fds);

	struct timeval tm;
	tm.tv_sec = timeout_sec;
	tm.tv_usec = timeout_usec;

	return select(sock + 1, &fds, NULL, NULL, &tm) > 0;
}

static bool knx_dgramsock_send_raw(int sock, const void* buffer, size_t buffer_size,
                                   const ip4addr* target) {
	ssize_t r = sendto(sock, buffer, buffer_size, 0,
	                   (const struct sockaddr*) target, sizeof(ip4addr));

	if (r < 0)
		return false;
	else
		return (size_t) r == buffer_size;
}

static bool knx_dgramsock_valid_sender(const ip4addr* endpoints, size_t num_endpoints,
                                       const ip4addr* sender, socklen_t sender_len) {
	if (!endpoints || !num_endpoints)
		return true;

	if (sender_len != sizeof(ip4addr) || sender->sin_family != AF_INET)
		return false;

	for (size_t i = 0; i < num_endpoints; i++) {
		if (endpoints[i].sin_addr.s_addr == sender->sin_addr.s_addr &&
		    endpoints[i].sin_port        == sender->sin_port)
				return true;
	}

	return false;
}

ssize_t knx_dgramsock_recv_raw(int sock, void* buffer, size_t buffer_size,
                               const ip4addr* endpoints, size_t num_endpoints) {
	ip4addr remote;
	socklen_t remote_size = sizeof(remote);

	ssize_t rv = recvfrom(sock, buffer, buffer_size, 0, (struct sockaddr*) &remote, &remote_size);

	if (rv > 0 && knx_dgramsock_valid_sender(endpoints, num_endpoints, &remote, remote_size))
		return rv;
	else
		return -1;
}

bool knx_dgramsock_recv(int sock, uint8_t* buffer, size_t size,
                        knx_packet* packet,
                        const ip4addr* endpoints, size_t num_endpoints) {
	ssize_t rv = knx_dgramsock_recv_raw(sock, buffer, size, endpoints, num_endpoints);
	return rv > 0 && knx_parse(buffer, rv, packet);
}

bool knx_dgramsock_send(int sock, knx_service srv, const void* payload, const ip4addr* target) {
	size_t size = knx_size(srv, payload);
	uint8_t buffer[size];

	if (!knx_generate(buffer, srv, payload))
		return false;

	return knx_dgramsock_send_raw(sock, buffer, size, target);
}

ssize_t knx_dgramsock_peek_knx(int sock) {
	ip4addr sender;
	socklen_t sender_len = sizeof(sender);

	uint8_t buffer[KNX_HEADER_SIZE];
	if (recvfrom(sock, buffer, KNX_HEADER_SIZE, MSG_PEEK,
	    (struct sockaddr*) &sender, &sender_len) == KNX_HEADER_SIZE) {

		size_t length;
		if (knx_unpack_header(buffer, NULL, &length))
			return length >= KNX_HEADER_SIZE ? length : 0;
	}

	return -1;
}

bool knx_socket_make_nonblocking(int fd) {
	return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) == 0;
}

bool knx_socket_make_blocking(int fd) {
	return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~O_NONBLOCK) == 0;
}

bool knx_socket_is_nonblocking(int fd) {
	return (fcntl(fd, F_GETFL, 0) & O_NONBLOCK) == O_NONBLOCK;
}
