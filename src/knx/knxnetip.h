#ifndef HPKNXD_KNX_KNXNETIP_H
#define HPKNXD_KNX_KNXNETIP_H

#include "header.h"
#include "connreq.h"

#include "../msgbuilder.h"

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

/**
 * KNXnet/IP Packet
 */
typedef struct {
	knxnetip_service service;
	union {
		knxnetip_connection_request conn_req;
	};
} knxnetip_packet;

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
