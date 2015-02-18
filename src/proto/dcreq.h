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

#ifndef KNXCLIENT_PROTO_DCREQ_H
#define KNXCLIENT_PROTO_DCREQ_H

#include "hostinfo.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Disconnect Request
 */
typedef struct {
	uint8_t channel;
	uint8_t status;
	knx_host_info host;
} knx_disconnect_request;

/**
 * Generate the message for a disconnect request.
 */
void knx_generate_disconnect_request(uint8_t* buffer, const knx_disconnect_request* req);

/**
 * Parse a message (excluding header) which contains a disconnect request.
 */
bool knx_parse_disconnect_request(const uint8_t* message, size_t length, knx_disconnect_request* req);

/**
 * Disconnect request size
 */
#define KNX_DISCONNECT_REQUEST_SIZE (2 + KNX_HOST_INFO_SIZE)

#endif
