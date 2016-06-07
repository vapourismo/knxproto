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

inline static
bool knx_dpt_bool_parse(const uint8_t* apdu, size_t length, knx_bool* value) {
	if (length != 1)
		return false;

	*value = *apdu & 1;

	return true;
}

inline static
bool knx_dpt_cvalue_parse(const uint8_t* apdu, size_t length, knx_cvalue* value) {
	if (length != 1)
		return false;

	value->control = *apdu >> 1 & 1;
	value->value = *apdu & 1;

	return true;
}

inline static
bool knx_dpt_cstep_parse(const uint8_t* apdu, size_t length, knx_cstep* value) {
	if (length != 1)
		return false;

	value->control = *apdu >> 3 & 1;
	value->step = *apdu & 7;

	return true;
}

inline static
bool knx_dpt_float16_parse(const uint8_t* apdu, size_t length, knx_float16* value) {
	if (length != 3)
		return false;

	// Mantissa
	int16_t m = (apdu[1] & 7) << 8 | apdu[2];

	// Signed?
	if (apdu[1] & 128)
		m -= 2048;

	// Exponent
	uint16_t e = apdu[1] >> 3 & 15;

	*value = 0.01 * ((float) m) * exp2f(e);

	return true;
}

inline static
bool knx_dpt_timeofday_parse(const uint8_t* apdu, size_t length, knx_timeofday* value) {
	if (length != 4)
		return false;

	value->day = apdu[1] >> 5 & 7;
	value->hour = (apdu[1] & 31) % 24;
	value->minute = (apdu[2] & 63) % 60;
	value->second = (apdu[3] & 63) % 60;

	return true;
}

inline static
bool knx_dpt_date_parse(const uint8_t* apdu, size_t length, knx_date* value) {
	if (length != 4)
		return false;

	// At least the KNX guys are not retarded
	value->day = (apdu[1] & 31) % 32;
	value->month = (apdu[2] & 15) % 13;
	value->year = (apdu[3] & 127) % 100;

	return true;
}

#define knx_dpt_as_is_parse(type) {           \
	if (length < sizeof(type) + 1)            \
		return false;                         \
	memcpy(result, apdu + 1, sizeof(type));   \
}

bool knx_dpt_from_apdu(const uint8_t* apdu, size_t length, knx_dpt type, void* result) {
	switch (type) {
		case KNX_DPT_BOOL:
			return knx_dpt_bool_parse(apdu, length, result);

		case KNX_DPT_CVALUE:
			return knx_dpt_cvalue_parse(apdu, length, result);

		case KNX_DPT_CSTEP:
			return knx_dpt_cstep_parse(apdu, length, result);

		case KNX_DPT_CHAR:
			knx_dpt_as_is_parse(knx_char);
			return true;

		case KNX_DPT_UNSIGNED8:
			knx_dpt_as_is_parse(knx_unsigned8);
			return true;

		case KNX_DPT_SIGNED8:
			knx_dpt_as_is_parse(knx_signed8);
			return true;

		case KNX_DPT_UNSIGNED16:
			knx_dpt_as_is_parse(knx_unsigned16);
			return true;

		case KNX_DPT_SIGNED16:
			knx_dpt_as_is_parse(knx_signed16);
			return true;

		case KNX_DPT_FLOAT16:
			return knx_dpt_float16_parse(apdu, length, result);

		case KNX_DPT_TIMEOFDAY:
			return knx_dpt_timeofday_parse(apdu, length, result);

		case KNX_DPT_DATE:
			return knx_dpt_date_parse(apdu, length, result);

		case KNX_DPT_UNSIGNED32:
			knx_dpt_as_is_parse(knx_unsigned32);
			return true;

		case KNX_DPT_SIGNED32:
			knx_dpt_as_is_parse(knx_signed32);
			return true;

		case KNX_DPT_FLOAT32:
			knx_dpt_as_is_parse(knx_float32);
			return true;

		default:
			return false;
	}
}

inline static
void knx_dpt_generate_bool(uint8_t* apdu, const bool* value) {
	apdu[0] &= ~63;
	apdu[0] |= *value & 1;
}

inline static
void knx_dpt_generate_cvalue(uint8_t* apdu, const knx_cvalue* value) {
	apdu[0] &= ~63;
	apdu[0] |= (value->control & 1) << 1 | (value->value & 1);
}

inline static
void knx_dpt_generate_cstep(uint8_t* apdu, const knx_cstep* value) {
	apdu[0] &= ~63;
	apdu[0] |= (value->control & 1) << 3 | (value->step & 7);
}

inline static
void knx_dpt_generate_timeofday(uint8_t* apdu, const knx_timeofday* value) {
	apdu[0] &= ~63;
	apdu[1] = (value->day & 7) >> 5 | (value->hour % 24);
	apdu[2] = value->minute % 60;
	apdu[3] = value->second % 60;
}

inline static
void knx_dpt_generate_date(uint8_t* apdu, const knx_date* value) {
	apdu[0] &= ~63;
	apdu[1] = value->day % 32;
	apdu[2] = value->month % 13;
	apdu[3] = value->year % 100;
}

inline static
void knx_dpt_generate_float16(uint8_t* apdu, const knx_float16* value) {
	apdu[0] &= ~63;

	float target = *value * 100;

	if (target > 67076096.0f)
		target = 67076096.0f;
	else if (target < -67108864.0f)
		target = -67108864.0f;

	uint8_t e = 0;

	// This isn't particularly efficient, but
	// has yielded the best results so far.
	while (target < -2048 || target > 2047) {
		target /= 2;
		e++;
	}

	int16_t m = target;

	if (m < 0)
		m = ~(~m + 1);

	apdu[1] = (m < 0) << 7
	        | (e & 15) << 3
	        | (m >> 8 & 7);
	apdu[2] = m & 255;
}


#define knx_dpt_generate_as_is(type) {      \
	apdu[0] &= ~63;                         \
	memcpy(apdu + 1, source, sizeof(type)); \
}

void knx_dpt_to_apdu(uint8_t* apdu, knx_dpt type, const void* source) {
	switch (type) {
		case KNX_DPT_BOOL:
			knx_dpt_generate_bool(apdu, source);
			break;

		case KNX_DPT_CVALUE:
			knx_dpt_generate_cvalue(apdu, source);
			break;

		case KNX_DPT_CSTEP:
			knx_dpt_generate_cstep(apdu, source);
			break;

		case KNX_DPT_CHAR:
			knx_dpt_generate_as_is(knx_char);
			break;

		case KNX_DPT_UNSIGNED8:
			knx_dpt_generate_as_is(knx_unsigned8);
			break;

		case KNX_DPT_SIGNED8:
			knx_dpt_generate_as_is(knx_signed8);
			break;

		case KNX_DPT_UNSIGNED16:
			knx_dpt_generate_as_is(knx_unsigned16);
			break;

		case KNX_DPT_SIGNED16:
			knx_dpt_generate_as_is(knx_signed16);
			break;

		case KNX_DPT_FLOAT16:
			knx_dpt_generate_float16(apdu, source);
			break;

		case KNX_DPT_TIMEOFDAY:
			knx_dpt_generate_timeofday(apdu, source);
			break;

		case KNX_DPT_DATE:
			knx_dpt_generate_date(apdu, source);
			break;

		case KNX_DPT_UNSIGNED32:
			knx_dpt_generate_as_is(knx_unsigned32);
			break;

		case KNX_DPT_SIGNED32:
			knx_dpt_generate_as_is(knx_signed32);
			break;

		case KNX_DPT_FLOAT32:
			knx_dpt_generate_as_is(knx_float32);
			break;

		default:
			break;
	}
}
