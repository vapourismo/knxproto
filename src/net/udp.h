#ifndef HPKNXD_NET_UDP_H
#define HPKNXD_NET_UDP_H

#include "address.h"

#include <stddef.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>

/**
 * Create a new UDP socket.
 */
int udpsock_create(const ip4addr* local);

/**
 * Send something through the socket.
 */
ssize_t udpsock_send(int sock, const void* data, size_t size, const ip4addr* remote);

/**
 * Receive a datagram from anybody.
 */
inline ssize_t udpsock_recv(int sock, void* buffer, size_t size) {
	return recvfrom(sock, buffer, size, 0, NULL, NULL);
}

/**
 * Receive a datagram from from specific remote address.
 */
ssize_t udpsock_recv_from(int sock, void* buffer, size_t size, const ip4addr* remote);

/**
 * Close the socket.
 */
inline void udpsock_close(int sock) {
	close(sock);
}

/**
 * Check if data is available. Wait a maximum of `timeout_sec` seconds for an answer.
 */
inline bool udpsock_select(int sock, time_t timeout_sec) {
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(sock, &fds);

	struct timeval tm;
	tm.tv_sec = timeout_sec;
	tm.tv_usec = 0;

	return select(sock + 1, &fds, NULL, NULL, &tm) > 0;
}

#endif
