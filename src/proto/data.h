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

#ifndef KNXPROTO_PROTO_DATA_H_
#define KNXPROTO_PROTO_DATA_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Datapoint Types
 */
typedef enum {
	/**
	 * DPT 1.xxx
	 * \see knx_bool
	 */
	KNX_DPT_BOOL,

	/**
	 * DPT 2.xxx
	 * \see knx_cvalue
	 */
	KNX_DPT_CVALUE,

	/**
	 * DPT 3.xxx
	 * \see knx_cstep
	 */
	KNX_DPT_CSTEP,

	/**
	 * DPT 4.xxx
	 * \see knx_char
	 */
	KNX_DPT_CHAR,

	/**
	 * DPT 5.xxx
	 * \see knx_unsigned8
	 */
	KNX_DPT_UNSIGNED8,

	/**
	 * DPT 6.xxx
	 * \see knx_signed8
	 */
	KNX_DPT_SIGNED8,

	/**
	 * DPT 7.xxx
	 * \see knx_unsigned16
	 */
	KNX_DPT_UNSIGNED16,

	/**
	 * DPT 8.xxx
	 * \see knx_signed16
	 */
	KNX_DPT_SIGNED16,

	/**
	 * DPT 9.xxx
	 * \see knx_float16
	 */
	KNX_DPT_FLOAT16,

	/**
	 * DPT 10.xxx
	 * \see knx_timeofday
	 */
	KNX_DPT_TIMEOFDAY,

	/**
	 * DPT 11.xxx
	 * \see knx_date
	 */
	KNX_DPT_DATE,

	/**
	 * DPT 12.xxx
	 * \see knx_unsigned32
	 */
	KNX_DPT_UNSIGNED32,

	/**
	 * DPT 13.xxx
	 * \see knx_signed32
	 */
	KNX_DPT_SIGNED32,

	/**
	 * DPT 14.xxx
	 * \see knx_float32
	 */
	KNX_DPT_FLOAT32,
} knx_dpt;

/**
 * DPT 1.xxx
 * \see KNX_DPT_BOOL
 */
typedef bool knx_bool;

/**
 * DPT 2.xxx
 * \see KNX_DPT_CVALUE
 */
typedef struct {
	bool control, value;
} knx_cvalue;

/**
 * DPT 3.xxx
 * \see KNX_DPT_CSTEP
 */
typedef struct {
	bool control;
	uint8_t step;
} knx_cstep;

/**
 * DPT 4.xxx
 * \see KNX_DPT_CHAR
 */
typedef char knx_char;

/**
 * DPT 5.xxx
 * \see KNX_DPT_UNSIGNED8
 */
typedef uint8_t knx_unsigned8;

/**
 * DPT 6.xxx
 * \see KNX_DPT_SIGNED8
 */
typedef int8_t knx_signed8;

/**
 * DPT 7.xxx
 * \see KNX_DPT_UNSIGNED16
 */
typedef uint16_t knx_unsigned16;

/**
 * DPT 8.xxx
 * \see KNX_DPT_SIGNED16
 */
typedef int16_t knx_signed16;

/**
 * DPT 9.xxx
 * \see KNX_DPT_FLOAT16
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
 * \see KNX_DPT_TIMEOFDAY
 */
typedef struct {
	knx_dayofweek day;
	uint8_t hour, minute, second;
} knx_timeofday;

/**
 * DPT 11.xxx
 * \see KNX_DPT_DATE
 */
typedef struct {
	uint8_t day, month, year;
} knx_date;

/**
 * DPT 12.xxx
 * \see KNX_DPT_UNSIGNED32
 */
typedef uint32_t knx_unsigned32;

/**
 * DPT 13.xxx
 * \see KNX_DPT_SIGNED32
 */
typedef int32_t knx_signed32;

/**
 * DPT 14.xxx
 * \see KNX_DPT_FLOAT32
 */
typedef float knx_float32;

/**
 * Interpret APDU in the given way to produce an instance of a C type.
 */
bool knx_dpt_from_apdu(const uint8_t* apdu, size_t length, knx_dpt type, void* result);

/**
 * Generate the APDU representation of the given type.
 */
void knx_dpt_to_apdu(uint8_t* apdu, knx_dpt type, const void* value);

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

/**
 * APDU size needed to fit an instance of the given datapoint type.
 */
inline static
size_t knx_dpt_size(knx_dpt type) {
	switch (type) {
		case KNX_DPT_BOOL:
			return KNX_DPT_BOOL_SIZE;

		case KNX_DPT_CVALUE:
			return KNX_DPT_CVALUE_SIZE;

		case KNX_DPT_CSTEP:
			return KNX_DPT_CSTEP_SIZE;

		case KNX_DPT_CHAR:
			return KNX_DPT_CHAR_SIZE;

		case KNX_DPT_UNSIGNED8:
			return KNX_DPT_UNSIGNED8_SIZE;

		case KNX_DPT_SIGNED8:
			return KNX_DPT_SIGNED8_SIZE;

		case KNX_DPT_UNSIGNED16:
			return KNX_DPT_UNSIGNED16_SIZE;

		case KNX_DPT_SIGNED16:
			return KNX_DPT_SIGNED16_SIZE;

		case KNX_DPT_FLOAT16:
			return KNX_DPT_FLOAT16_SIZE;

		case KNX_DPT_TIMEOFDAY:
			return KNX_DPT_TIMEOFDAY_SIZE;

		case KNX_DPT_DATE:
			return KNX_DPT_DATE_SIZE;

		case KNX_DPT_UNSIGNED32:
			return KNX_DPT_UNSIGNED32_SIZE;

		case KNX_DPT_SIGNED32:
			return KNX_DPT_SIGNED32_SIZE;

		case KNX_DPT_FLOAT32:
			return KNX_DPT_FLOAT32_SIZE;

		default:
			return 0;
	}
}

#endif
