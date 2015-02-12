#ifndef HPKNXD_KNX_KNXNETIP_H
#define HPKNXD_KNX_KNXNETIP_H

#include "common.h"
#include "../msgbuilder.h"

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <netinet/in.h>

/**
 * Host Information
 */
typedef struct {
	knxnetip_proto protocol;
	in_addr_t address;
	in_port_t port;
} knxnetip_host_info;

/**
 * Connection Request
 */
typedef struct {
	knxnetip_conn_type type;
	knxnetip_layer layer;
	knxnetip_host_info control_host;
	knxnetip_host_info tunnel_host;
} knxnetip_connreq;

/**
 * KNXnet/IP Packet
 */
typedef struct {
	knxnetip_service service;
	union {
		knxnetip_connreq connection_request;
	};
} knxnetip_packet;

/**
 * Generate the message for a connection request.
 */
bool knxnetip_generate_connreq(msgbuilder* mb, const knxnetip_connreq* connreq);

/**
 * Generate the message from the given packet information.
 */
bool knxnetip_generate(msgbuilder* mb, const knxnetip_packet* packet);

/**
 * Parse a given message into a packet.
 */
bool knxnetip_parse(const uint8_t* msg, size_t length,
                    knxnetip_packet* packet);

#endif
