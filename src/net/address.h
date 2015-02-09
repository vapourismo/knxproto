#ifndef HPKNXD_NET_ADDRESS_H
#define HPKNXD_NET_ADDRESS_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

/**
 * IPv4 Address
 */
typedef struct sockaddr_in ip4address;

/**
 * Generate an IPv4 address from a string and a given port.
 */
inline void ip4address_from_string(ip4address* addr, const char* addrstr, uint16_t port) {
	memset(addr, 0, sizeof(ip4address));
	inet_pton(AF_INET, addrstr, &addr->sin_addr);
	addr->sin_port = htons(port);
}

#endif
