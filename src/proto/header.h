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

#ifndef KNXCLIENT_PROTO_HEADER_H
#define KNXCLIENT_PROTO_HEADER_H

#include "../util/msgbuilder.h"

#include <stdint.h>
#include <stdbool.h>

/**
 * KNXnet/IP Service Type
 */
typedef enum {
	KNX_SEARCH_REQUEST               = 0x0201,
	KNX_SEARCH_RESPONSE              = 0x0202,
	KNX_DESCRIPTION_REQUEST          = 0x0203,
	KNX_DESCRIPTION_RESPONSE         = 0x0204,
	KNX_CONNECTION_REQUEST           = 0x0205,
	KNX_CONNECTION_RESPONSE          = 0x0206,
	KNX_CONNECTIONSTATE_REQUEST      = 0x0207,
	KNX_CONNECTIONSTATE_RESPONSE     = 0x0208,
	KNX_DISCONNECT_REQUEST           = 0x0209,
	KNX_DISCONNECT_RESPONSE          = 0x020A,
	KNX_DEVICE_CONFIGURATION_REQUEST = 0x0310,
	KNX_DEVICE_CONFIGURATION_ACK     = 0x0311,
	KNX_TUNNEL_REQUEST               = 0x0420,
	KNX_TUNNEL_RESPONSE              = 0x0421,
	KNX_ROUTING_INDICATION           = 0x0530
} knx_service;

/**
 * Append a KNXnet/IP header which advocates the given service and payload length.
 */
bool knx_append_header(msgbuilder* mb, knx_service srv, uint16_t length);

#endif
