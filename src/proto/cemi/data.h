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

#ifndef KNXCLIENT_PROTO_CEMI_DATA_H
#define KNXCLIENT_PROTO_CEMI_DATA_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Datapoint Types
 */
typedef enum {
	KNX_DPT_B1,
	KNX_DPT_B2,
	KNX_DPT_B1U3,
	KNX_DPT_A8,
	KNX_DPT_U8,
	KNX_DPT_V8,
	KNX_DPT_U16,
} knx_datapoint_type;

/**
 * B1 Datapoint (DPT 1.xxx)
 */
typedef bool knx_b1;

/**
 * B2 Datapoint (DPT 2.xxx)
 */
typedef struct {
	bool control, value;
} knx_b2;

/**
 * B2 Datapoint (DPT 3.xxx)
 */
typedef struct {
	bool control;
	uint8_t step;
} knx_b1u3;

/**
 * A8 Datapoint (DPT 4.xxx)
 */
typedef char knx_a8;

/**
 * U8 Datapoint (DPT 5.xxx)
 */
typedef uint8_t knx_u8;

/**
 * V8 Datapoint (DPT 6.xxx)
 */
typedef int8_t knx_v8;

/**
 * U16 Datapoint (DPT 7.xxx)
 */
typedef uint16_t knx_u16;

/**
 * Get an instance of a datapoint from the APDU.
 */
bool knx_datapoint_from_apdu(const uint8_t* apdu, size_t length, knx_datapoint_type type, void* result);

#endif
