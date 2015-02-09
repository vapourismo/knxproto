#ifndef HPKNXD_NET_UDP_H
#define HPKNXD_NET_UDP_H

#include "address.h"

#include <stddef.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/socket.h>

/**
 * Create a new UDP socket.
 */
int udpsock_create(const ip4addr* local);

/**
 *
 */
inline ssize_t udpsock_send(int sock, const void* data, size_t size, const ip4addr* remote) {
	return sendto(sock, data, size, 0,
	              (const struct sockaddr*) remote, sizeof(ip4addr));
}

/**
 *
 */
inline ssize_t udpsock_recv(int sock, void* buffer, size_t size) {
	return recvfrom(sock, buffer, size, 0,
	                NULL, NULL);
}

/**
 *
 */
inline void udpsock_close(int sock) {
	close(sock);
}

#endif
