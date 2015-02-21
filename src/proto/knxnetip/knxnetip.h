/* KNX Client Library
 * A library which provides the means to communicate with several
 * KNX-related devices or services.
 *
 * Copyright (C) 2014, Ole Krüger <ole@vprsm.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef KNXCLIENT_PROTO_KNXNETIP_KNXNETIP_H
#define KNXCLIENT_PROTO_KNXNETIP_KNXNETIP_H

#include "header.h"
#include "connreq.h"
#include "connres.h"
#include "connstatereq.h"
#include "connstateres.h"
#include "dcreq.h"
#include "dcres.h"
#include "tunnelreq.h"
#include "tunnelres.h"
#include "routingind.h"

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
		knx_routing_indication routing_ind;
	} payload;
} knx_packet;

/**
 * Parse a given message into a packet.
 */
bool knx_parse(const uint8_t* msg, size_t length,
               knx_packet* packet);

/**
 * Generate a message.
 */
bool knx_generate(uint8_t* buffer, knx_service service, const void* payload);

/**
 * Calculate the space needed to generate a message.
 * This excludes the space needed for a header.
 */
size_t knx_payload_size(knx_service service, const void* payload);

#endif
