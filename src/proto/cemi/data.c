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

#include "data.h"

#include <math.h>
#include <string.h>

inline static bool knx_dpt_parse_bool(const uint8_t* apdu, size_t length, knx_bool* value) {
	if (length != 1)
		return false;

	*value = *apdu & 1;

	return true;
}

inline static bool knx_dpt_parse_cvalue(const uint8_t* apdu, size_t length, knx_cvalue* value) {
	if (length != 1)
		return false;

	value->control = *apdu >> 1 & 1;
	value->value = *apdu & 1;

	return true;
}

inline static bool knx_dpt_parse_cstep(const uint8_t* apdu, size_t length, knx_cstep* value) {
	if (length != 1)
		return false;

	value->control = *apdu >> 3 & 1;
	value->step = *apdu & 7;

	return true;
}

inline static bool knx_dpt_parse_float16(const uint8_t* apdu, size_t length, knx_float16* value) {
	if (length != 3)
		return false;

	// Mantissa
	int16_t m = (apdu[1] & 7) << 8 | apdu[2];

	// Signed?
	if (apdu[1] & 128)
		m -= 2048;

	// Exponent
	uint16_t e = apdu[1] >> 3 & 15;

	*value = 0.01 * ((float) m) * pow(2, (float) e);

	return true;
}

inline static bool knx_dpt_parse_timeofday(const uint8_t* apdu, size_t length, knx_timeofday* value) {
	if (length != 4)
		return false;

	value->day = apdu[1] >> 5 & 7;
	value->hour = (apdu[1] & 31) % 24;
	value->minute = (apdu[2] & 63) % 60;
	value->second = (apdu[3] & 63) % 60;

	return true;
}

inline static bool knx_dpt_parse_date(const uint8_t* apdu, size_t length, knx_date* value) {
	if (length != 4)
		return false;

	// At least the KNX guys are not retarded
	value->day = (apdu[1] & 31) % 32;
	value->month = (apdu[2] & 15) % 13;
	value->year = (apdu[3] & 127) % 100;

	return true;
}

#define knx_dpt_parse_as_is(type) {               \
	if ((length) < sizeof(type) + 1)              \
		return false;                             \
	memcpy((result), (apdu) + 1, sizeof(type));   \
	return true;                                  \
}

bool knx_datapoint_from_apdu(const uint8_t* apdu, size_t length, knx_datapoint_type type, void* result) {
	switch (type) {
		case KNX_DPT_BOOL:
			return knx_dpt_parse_bool(apdu, length, result);

		case KNX_DPT_CVALUE:
			return knx_dpt_parse_cvalue(apdu, length, result);

		case KNX_DPT_CSTEP:
			return knx_dpt_parse_cstep(apdu, length, result);

		case KNX_DPT_CHAR:
			knx_dpt_parse_as_is(knx_char);

		case KNX_DPT_UNSIGNED8:
			knx_dpt_parse_as_is(knx_unsigned8);

		case KNX_DPT_SIGNED8:
			knx_dpt_parse_as_is(knx_signed8);

		case KNX_DPT_UNSIGNED16:
			knx_dpt_parse_as_is(knx_unsigned16);

		case KNX_DPT_SIGNED16:
			knx_dpt_parse_as_is(knx_signed16);

		case KNX_DPT_FLOAT16:
			return knx_dpt_parse_float16(apdu, length, result);

		case KNX_DPT_TIMEOFDAY:
			return knx_dpt_parse_timeofday(apdu, length, result);

		case KNX_DPT_DATE:
			return knx_dpt_parse_date(apdu, length, result);

		case KNX_DPT_UNSIGNED32:
			knx_dpt_parse_as_is(knx_unsigned32);

		case KNX_DPT_SIGNED32:
			knx_dpt_parse_as_is(knx_signed32);

		case KNX_DPT_FLOAT32:
			knx_dpt_parse_as_is(knx_float32);

		default:
			return false;
	}
}
