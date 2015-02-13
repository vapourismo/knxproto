#include "udp.h"

int udpsock_create(const ip4addr* local) {
	int sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock < 0)
		return sock;

	if (local && bind(sock, (const struct sockaddr*) local,
	                  sizeof(ip4addr)) != 0) {
		udpsock_close(sock);
		return -1;
	}

	return sock;
}

bool udpsock_send(int sock, const void* data, size_t size, const ip4addr* remote) {
	size_t sent = 0;

	do {
		ssize_t tmp = sendto(sock, data, size, 0,
		                     (const struct sockaddr*) remote, sizeof(ip4addr));

		if (tmp < 0)
			return false;

		sent += tmp;
	} while (sent < size);

	return true;
}

ssize_t udpsock_recv_from(int sock, void* buffer, size_t size, const ip4addr* remote) {
	ip4addr other;
	socklen_t other_len = sizeof(other);

	while (true) {
		ssize_t r = recvfrom(sock, buffer, size, 0, (struct sockaddr*) &other, &other_len);

		if (r < 0)
			return r;

		if (other_len == sizeof(ip4addr) &&
		    other.sin_family == remote->sin_family &&
		    other.sin_addr.s_addr == remote->sin_addr.s_addr &&
		    other.sin_port == remote->sin_port)
		    	return r;
	}
}
