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

#ifndef KNXCLIENT_PROTO_KNXNETIP_DESCRES_H
#define KNXCLIENT_PROTO_KNXNETIP_DESCRES_H

#include "../util/address.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
	uint8_t family, version;
} knx_description_service;

/**
 * Description Response
 */
typedef struct {
	uint8_t medium, status;
	knx_addr address;
	uint16_t id;
	knx_description_service* services;
	size_t num_services;
} knx_description_response;

/**
 * Generate the message for a description response.
 */
void knx_description_response_generate(uint8_t* buffer, const knx_description_response* res);

/**
 * Parse a message (excluding header) which contains a description response.
 * Note: You have to free the `services` array manually.
 */
bool knx_description_response_parse(const uint8_t* message, size_t length, knx_description_response* res);

/**
 * Description response size
 */
inline size_t knx_description_response_size(const knx_description_response* res) {
	return 0;
}

#endif
