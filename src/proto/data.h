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
	KNX_DPT_BOOL,
	KNX_DPT_CVALUE,
	KNX_DPT_CSTEP,
	KNX_DPT_CHAR,
	KNX_DPT_UNSIGNED8,
	KNX_DPT_SIGNED8,
	KNX_DPT_UNSIGNED16,
	KNX_DPT_SIGNED16,
	KNX_DPT_FLOAT16,
	KNX_DPT_TIMEOFDAY,
	KNX_DPT_DATE,
	KNX_DPT_UNSIGNED32,
	KNX_DPT_SIGNED32,
	KNX_DPT_FLOAT32,
} knx_dpt;

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
 * DPT 11.xxx
 */
typedef struct {
	uint8_t day, month, year;
} knx_date;

/**
 * DPT 12.xxx
 */
typedef uint32_t knx_unsigned32;

/**
 * DPT 13.xxx
 */
typedef int32_t knx_signed32;

/**
 * DPT 14.xxx
 */
typedef float knx_float32;

/**
 * Interpret APDU in the given way to produce an instance of a C type.
 */
bool knx_dpt_from_apdu(const uint8_t* apdu, size_t length, knx_dpt type, void* result);

/**
 * Generate the APDU representation of the given type. This function leaves
 * the first 2 bits (least significant bits of the APCI) untouched.
 */
void knx_dpt_to_apdu(uint8_t* apdu, knx_dpt type, const void* result);

/**
 * APDU size for `bool`
 */
#define KNX_DPT_BOOL_SIZE       1

/**
 * APDU size for `cvalue`
 */
#define KNX_DPT_CVALUE_SIZE     1

/**
 * APDU size for `cstep`
 */
#define KNX_DPT_CSTEP_SIZE      1

/**
 * APDU size for `char`
 */
#define KNX_DPT_CHAR_SIZE       2

/**
 * APDU size for `unsigned8`
 */
#define KNX_DPT_UNSIGNED8_SIZE  2

/**
 * APDU size for `signed8`
 */
#define KNX_DPT_SIGNED8_SIZE    2

/**
 * APDU size for `unsigned16`
 */
#define KNX_DPT_UNSIGNED16_SIZE 3

/**
 * APDU size for `signed16`
 */
#define KNX_DPT_SIGNED16_SIZE   3

/**
 * APDU size for `float16`
 */
#define KNX_DPT_FLOAT16_SIZE    3

/**
 * APDU size for `timeofday`
 */
#define KNX_DPT_TIMEOFDAY_SIZE  4

/**
 * APDU size for `date`
 */
#define KNX_DPT_DATE_SIZE       4

/**
 * APDU size for `unsigned32`
 */
#define KNX_DPT_UNSIGNED32_SIZE 5

/**
 * APDU size for `signed32`
 */
#define KNX_DPT_SIGNED32_SIZE   5

/**
 * APDU size for `float32`
 */
#define KNX_DPT_FLOAT32_SIZE    5

#endif
