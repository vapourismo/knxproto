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
	KNX_DPT_1,
	KNX_DPT_2,
	KNX_DPT_3,
	KNX_DPT_4,
	KNX_DPT_5,
	KNX_DPT_6,
	KNX_DPT_7,
	KNX_DPT_8,
	KNX_DPT_9,
	KNX_DPT_10,
} knx_datapoint_type;

/**
 * DPT 1.xxx
 */
typedef bool knx_bool;

/**
 * DPT 2.xxx
 */
typedef struct {
	bool control, value;
} knx_cvalue;

/**
 * DPT 3.xxx
 */
typedef struct {
	bool control;
	uint8_t step;
} knx_cstep;

/**
 * DPT 4.xxx
 */
typedef char knx_char;

/**
 * DPT 5.xxx
 */
typedef uint8_t knx_unsigned8;

/**
 * DPT 6.xxx
 */
typedef int8_t knx_signed8;

/**
 * DPT 7.xxx
 */
typedef uint16_t knx_unsigned16;

/**
 * DPT 8.xxx
 */
typedef int16_t knx_signed16;

/**
 * DPT 9.xxx
 */
typedef float knx_float16;

/**
 * Day of the week
 */
typedef enum {
	KNX_NODAY     = 0,
	KNX_MONDAY    = 1,
	KNX_TUESDAY   = 2,
	KNX_WEDNESDAY = 3,
	KNX_THURSDAY  = 4,
	KNX_FRIDAY    = 5,
	KNX_SATURDAY  = 6,
	KNX_SUNDAY    = 7
} knx_dayofweek;

/**
 * DPT 10.xxx
 */
typedef struct {
	knx_dayofweek day;
	uint8_t hour, minute, second;
} knx_timeofday;

/**
 * Get an instance of a datapoint from the APDU.
 */
bool knx_datapoint_from_apdu(const uint8_t* apdu, size_t length, knx_datapoint_type type, void* result);

#endif
