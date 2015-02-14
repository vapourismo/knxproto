#ifndef KNXCLIENT_NET_DGRAMSOCK
#define KNXCLIENT_NET_DGRAMSOCK

#include "address.h"

#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>

/**
 * Datagram Socket
 */
typedef int dgramsock;

/**
 * Create a datagram socket.
 */
dgramsock dgramsock_create(const ip4addr* local, bool reuse);

/**
 * Check if data is available.
 */
bool dgramsock_ready(dgramsock sock, time_t timeout_sec, time_t timeout_usec);

/**
 * Receive data from a number of given endpoints.
 */
ssize_t dgramsock_recv(dgramsock sock, void* buffer, size_t buffer_size,
                        const ip4addr* endpoints, size_t num_endpoints);

/**
 * Send a datagram.
 */
inline bool dgramsock_send(dgramsock sock, const void* buffer, size_t buffer_size,
                           const ip4addr* target) {
	ssize_t r = sendto(sock, buffer, buffer_size, 0,
	                   (const struct sockaddr*) target, sizeof(ip4addr));

	if (r < 0)
		return false;
	else
		return (size_t) r == buffer_size;
}

/**
 * Destroy the datagram socket.
 */
inline void dgramsock_close(dgramsock sock) {
	close(sock);
}

#endif
