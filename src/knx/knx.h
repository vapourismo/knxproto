#ifndef HPKNXD_KNX_KNX_H
#define HPKNXD_KNX_KNX_H

#include "header.h"
#include "connreq.h"
#include "connres.h"
#include "connstatereq.h"
#include "connstateres.h"
#include "dcreq.h"
#include "dcres.h"
#include "tunnelreq.h"
#include "tunnelres.h"

#include "../msgbuilder.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/**
 * KNXnet/IP Packet
 */
typedef struct {
	knx_service service;
	union {
		knx_connection_request conn_req;
		knx_connection_response conn_res;
		knx_disconnect_request dc_req;
		knx_disconnect_response dc_res;
		knx_connection_state_request conn_state_req;
		knx_connection_state_response conn_state_res;
		knx_tunnel_request tunnel_req;
		knx_tunnel_response tunnel_res;
	} payload;
} knx_packet;

/**
 * Parse a given message into a packet.
 */
bool knx_parse(const uint8_t* msg, size_t length,
                    knx_packet* packet);

#endif
