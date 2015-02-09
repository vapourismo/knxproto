#include "udp.h"

int udpsock_create(const ip4addr* local) {
	int sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock < 0)
		return false;

	if (local && bind(sock, (const struct sockaddr*) local,
	                  sizeof(ip4addr)) != 0) {
		udpsock_close(sock);
		return -1;
	}

	return sock;
}
