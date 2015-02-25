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
#include <sys/socket.h>
#include <sys/select.h>

int dgramsock_create(const ip4addr* local, bool reuse) {
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

bool dgramsock_ready(int sock, time_t timeout_sec, long timeout_usec) {
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(sock, &fds);

	struct timeval tm;
	tm.tv_sec = timeout_sec;
	tm.tv_usec = timeout_usec;

	return select(sock + 1, &fds, NULL, NULL, &tm) > 0;
}

inline bool dgramsock_valid_sender(const ip4addr* endpoints, size_t num_endpoints,
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

ssize_t dgramsock_recv(int sock, void* buffer, size_t buffer_size,
                       const ip4addr* endpoints, size_t num_endpoints) {
	ip4addr remote;
	socklen_t remote_size = sizeof(remote);

	ssize_t rv = recvfrom(sock, buffer, buffer_size, 0, (struct sockaddr*) &remote, &remote_size);

	if (rv > 0 && dgramsock_valid_sender(endpoints, num_endpoints, &remote, remote_size))
		return rv;
	else
		return -1;
}

bool dgramsock_send_knx(int sock, knx_service srv, const void* payload, const ip4addr* target) {
	size_t size = KNX_HEADER_SIZE + knx_payload_size(srv, payload);
	uint8_t buffer[size];

	if (!knx_generate(buffer, srv, payload))
		return false;

	return dgramsock_send(sock, buffer, size, target);
}

ssize_t dgramsock_peek_knx(int sock) {
	ip4addr sender;
	socklen_t sender_len = sizeof(sender);

	uint8_t buffer[KNX_HEADER_SIZE];
	if (recvfrom(sock, buffer, KNX_HEADER_SIZE, MSG_PEEK, &sender, &sender_len) == KNX_HEADER_SIZE) {
		uint16_t length;
		if (knx_unpack_header(buffer, NULL, &length))
			return length >= KNX_HEADER_SIZE ? length : 0;
	}

	return -1;
}
