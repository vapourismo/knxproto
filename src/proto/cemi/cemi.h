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

#ifndef KNXCLIENT_PROTO_CEMI_CEMI_H
#define KNXCLIENT_PROTO_CEMI_CEMI_H

#include "ldata.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/**
 * KNX CEMI Types
 */
typedef enum {
	KNX_CEMI_LDATA_REQ = 0x11,
	KNX_CEMI_LDATA_IND = 0x29
} knx_cemi_service;

/**
 * CEMI Frame
 */
typedef struct {
	knx_cemi_service service;
	uint8_t add_info_length;
	const uint8_t* add_info;
	union {
		knx_ldata ldata;
	} payload;
} knx_cemi_frame;

/**
 * CEMI Header Size
 */
#define KNX_CEMI_HEADER_SIZE 2

/**
 * Unpack CEMI header.
 */
void knx_cemi_unpack_header(const uint8_t* buffer, knx_cemi_service* service, uint8_t* info_length);

/**
 * Parse a message which contains a CEMI frame.
 */
bool knx_cemi_parse(const uint8_t* message, size_t length, knx_cemi_frame* frame);

/**
 * Generate a CEMI frame.
 */
bool knx_generate_cemi(uint8_t* buffer, knx_cemi_service service,
                       uint8_t* add_info, uint8_t add_info_length, const void* payload);

/**
 * Same as `knx_cemi_generate` but without any additional info.
 */
inline bool knx_cemi_generate_(uint8_t* buffer, knx_cemi_service service, const void* payload) {
	return knx_cemi_generate(buffer, service, NULL, 0, payload);
}

#endif
