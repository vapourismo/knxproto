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

inline static bool knx_dpt_parse_char(const uint8_t* apdu, size_t length, knx_char* value) {
	if (length != 2)
		return false;

	*value = apdu[1];

	return true;
}

inline static bool knx_dpt_parse_unsigned8(const uint8_t* apdu, size_t length, knx_unsigned8* value) {
	if (length != 2)
		return false;

	*value = apdu[1];

	return true;
}

inline static bool knx_dpt_parse_signed8(const uint8_t* apdu, size_t length, knx_signed8* value) {
	if (length != 2)
		return false;

	*value = apdu[1];

	return true;
}

inline static bool knx_dpt_parse_unsigned16(const uint8_t* apdu, size_t length, knx_unsigned16* value) {
	if (length != 3)
		return false;

	*value = apdu[1] << 8 | apdu[2];

	return true;
}

inline static bool knx_dpt_parse_signed16(const uint8_t* apdu, size_t length, knx_signed16* value) {
	if (length != 3)
		return false;

	*value = apdu[1] << 8 | apdu[2];

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

inline static bool knx_dpt_parse_unsigned32(const uint8_t* apdu, size_t length, knx_unsigned32* value) {
	if (length != 5)
		return false;

	*value = apdu[1] << 24 | apdu[2] << 16 | apdu[3] << 8 | apdu[4];

	return true;
}

inline static bool knx_dpt_parse_signed32(const uint8_t* apdu, size_t length, knx_signed32* value) {
	if (length != 5)
		return false;

	*value = apdu[1] << 24 | apdu[2] << 16 | apdu[3] << 8 | apdu[4];

	return true;
}

inline static bool knx_dpt_parse_float32(const uint8_t* apdu, size_t length, knx_float32* value) {
	if (length != sizeof(knx_float32) + 1)
		return false;

	// YOLO
	memcpy(value, apdu + 1, sizeof(knx_float32));

	return true;
}

bool knx_datapoint_from_apdu(const uint8_t* apdu, size_t length, knx_datapoint_type type, void* result) {
	switch (type) {
		case KNX_DPT_1:
			return knx_dpt_parse_bool(apdu, length, result);

		case KNX_DPT_2:
			return knx_dpt_parse_cvalue(apdu, length, result);

		case KNX_DPT_3:
			return knx_dpt_parse_cstep(apdu, length, result);

		case KNX_DPT_4:
			return knx_dpt_parse_char(apdu, length, result);

		case KNX_DPT_5:
			return knx_dpt_parse_unsigned8(apdu, length, result);

		case KNX_DPT_6:
			return knx_dpt_parse_signed8(apdu, length, result);

		case KNX_DPT_7:
			return knx_dpt_parse_unsigned16(apdu, length, result);

		case KNX_DPT_8:
			return knx_dpt_parse_signed16(apdu, length, result);

		case KNX_DPT_9:
			return knx_dpt_parse_float16(apdu, length, result);

		case KNX_DPT_10:
			return knx_dpt_parse_timeofday(apdu, length, result);

		case KNX_DPT_11:
			return knx_dpt_parse_date(apdu, length, result);

		case KNX_DPT_12:
			return knx_dpt_parse_unsigned32(apdu, length, result);

		case KNX_DPT_13:
			return knx_dpt_parse_signed32(apdu, length, result);

		case KNX_DPT_14:
			return knx_dpt_parse_float32(apdu, length, result);

		default:
			return false;
	}
}
