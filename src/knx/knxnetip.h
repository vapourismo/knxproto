#ifndef HPKNXD_KNX_KNXNETIP_H
#define HPKNXD_KNX_KNXNETIP_H

#include "header.h"
#include "hostinfo.h"

#include "../msgbuilder.h"

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

/**
 * KNX Connection Type
 */
typedef enum {
	KNXNETIP_conn_req_TUNNEL = 4
} knxnetip_conn_type;

/**
 * KNX Layer
 */
typedef enum {
	KNXNETIP_LAYER_TUNNEL = 2
} knxnetip_layer;

/**
 * Connection Request
 */
typedef struct {
	knxnetip_conn_type type;
	knxnetip_layer layer;
	knxnetip_host_info control_host;
	knxnetip_host_info tunnel_host;
} knxnetip_conn_req;

/**
 * KNXnet/IP Packet
 */
typedef struct {
	knxnetip_service service;
	union {
		knxnetip_conn_req connection_request;
	};
} knxnetip_packet;

/**
 * Generate the message for a connection request.
 */
bool knxnetip_generate_conn_req(msgbuilder* mb, const knxnetip_conn_req* conn_req);

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
