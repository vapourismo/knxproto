#ifndef HPKNXD_KNX_HOSTINFO_H
#define HPKNXD_KNX_HOSTINFO_H

#include "../msgbuilder.h"

#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * KNXnet/IP Protocol
 */
typedef enum {
	KNX_PROTO_UDP = 1,
	KNX_PROTO_TCP = 2
} knx_proto;

/**
 * Host Information
 */
typedef struct {
	knx_proto protocol;
	in_addr_t address;
	in_port_t port;
} knx_host_info;

/**
 * Append host information.
 */
bool knx_append_host_info(msgbuilder* mb, const knx_host_info* host);

/**
 * Retrieve host information.
 */
bool knx_parse_host_info(const uint8_t* message, knx_host_info* host);

#endif
