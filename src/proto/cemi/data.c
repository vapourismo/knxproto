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

inline static bool knx_dpt_parse_b1(const uint8_t* apdu, size_t length, knx_b1* value) {
	if (length != 1)
		return false;

	*value = *apdu & 1;

	return true;
}

inline static bool knx_dpt_parse_b2(const uint8_t* apdu, size_t length, knx_b2* value) {
	if (length != 1)
		return false;

	value->control = *apdu >> 1 & 1;
	value->value = *apdu & 1;

	return true;
}

inline static bool knx_dpt_parse_b1u3(const uint8_t* apdu, size_t length, knx_b1u3* value) {
	if (length != 1)
		return false;

	value->control = *apdu >> 3 & 1;
	value->step = *apdu & 7;

	return true;
}

inline static bool knx_dpt_parse_a8(const uint8_t* apdu, size_t length, knx_a8* value) {
	if (length != 2)
		return false;

	*value = apdu[1];

	return true;
}

inline static bool knx_dpt_parse_u8(const uint8_t* apdu, size_t length, knx_u8* value) {
	if (length != 2)
		return false;

	*value = apdu[1];

	return true;
}

inline static bool knx_dpt_parse_v8(const uint8_t* apdu, size_t length, knx_v8* value) {
	if (length != 2)
		return false;

	*value = apdu[1];

	return true;
}

inline static bool knx_dpt_parse_u16(const uint8_t* apdu, size_t length, knx_u16* value) {
	if (length != 3)
		return false;

	*value = apdu[1] << 8 | apdu[2];

	return true;
}

inline static bool knx_dpt_parse_v16(const uint8_t* apdu, size_t length, knx_v16* value) {
	if (length != 3)
		return false;

	*value = apdu[1] << 8 | apdu[2];

	return true;
}

bool knx_datapoint_from_apdu(const uint8_t* apdu, size_t length, knx_datapoint_type type, void* result) {
	switch (type) {
		case KNX_DPT_B1:
			return knx_dpt_parse_b1(apdu, length, result);

		case KNX_DPT_B2:
			return knx_dpt_parse_b2(apdu, length, result);

		case KNX_DPT_B1U3:
			return knx_dpt_parse_b1u3(apdu, length, result);

		case KNX_DPT_A8:
			return knx_dpt_parse_a8(apdu, length, result);

		case KNX_DPT_U8:
			return knx_dpt_parse_u8(apdu, length, result);

		case KNX_DPT_V8:
			return knx_dpt_parse_v8(apdu, length, result);

		case KNX_DPT_U16:
			return knx_dpt_parse_u16(apdu, length, result);

		case KNX_DPT_V16:
			return knx_dpt_parse_v16(apdu, length, result);

		default:
			return false;
	}
}
