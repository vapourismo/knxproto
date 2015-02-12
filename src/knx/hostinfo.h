#ifndef HPKNXD_KNX_HOSTINFO_H
#define HPKNXD_KNX_HOSTINFO_H

#include "../msgbuilder.h"

#include <netinet/in.h>
#include <stdbool.h>

/**
 * KNXnet/IP Protocol
 */
typedef enum {
	KNXNETIP_PROTO_UDP = 1,
	KNXNETIP_PROTO_TCP = 2
} knxnetip_proto;

/**
 * Host Information
 */
typedef struct {
	knxnetip_proto protocol;
	in_addr_t address;
	in_port_t port;
} knxnetip_host_info;

/**
 * Append host information.
 */
bool knxnetip_append_host_info(msgbuilder* mb, const knxnetip_host_info* host);

#endif
