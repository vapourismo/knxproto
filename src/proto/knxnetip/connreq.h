/* KNX Client Library
 * A library which provides the means to communicate with several
 * KNX-related devices or services.
 *
 * Copyright (C) 2014-2015, Ole Krüger <ole@vprsm.de>
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

#ifndef KNXCLIENT_PROTO_KNXNETIP_CONNREQ_H
#define KNXCLIENT_PROTO_KNXNETIP_CONNREQ_H

#include "hostinfo.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * KNX Connection Type
 */
typedef enum {
	KNX_CONNECTION_REQUEST_TUNNEL = 4
} knx_conn_type;

/**
 * KNX Layer
 */
typedef enum {
	KNX_LAYER_TUNNEL = 2
} knx_layer;

/**
 * Connection Request
 */
typedef struct {
	knx_conn_type type;
	knx_layer layer;
	knx_host_info control_host;
	knx_host_info tunnel_host;
} knx_connection_request;

/**
 * Generate the message for a connection request.
 */
void knx_connection_request_generate(uint8_t* buffer, const knx_connection_request* conn_req);

/**
 * Parse a message (excluding header) which contains a connection request.
 */
bool knx_parse_connection_request(const uint8_t* message, size_t length, knx_connection_request* req);

/**
 * Connection request size
 */
#define KNX_CONNECTION_REQUEST_SIZE (4 + KNX_HOST_INFO_SIZE * 2)

#endif
