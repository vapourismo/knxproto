#ifndef HPKNXD_KNX_KNXNETIP_H
#define HPKNXD_KNX_KNXNETIP_H

#include "header.h"
#include "connreq.h"
#include "connres.h"

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
		knxnetip_connection_response conn_res;
	} payload;
} knxnetip_packet;

/**
 * Parse a given message into a packet.
 */
bool knxnetip_parse(const uint8_t* msg, size_t length,
                    knxnetip_packet* packet);

#endif
