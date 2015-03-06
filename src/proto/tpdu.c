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

#include "tpdu.h"
#include "../util/alloc.h"

#include <string.h>

bool knx_tpdu_parse(const uint8_t* tpdu, size_t length, knx_tpdu* info) {
	if (length == 0)
		return false;

	info->tpci = tpdu[0] >> 6 & 3;
	info->seq_number = tpdu[0] >> 2 & 15;

	if (info->tpci == KNX_TPCI_UNNUMBERED_CONTROL || info->tpci == KNX_TPCI_NUMBERED_CONTROL) {
		info->info.control = tpdu[0] & 3;
	} else if (length < 2) {
		return false;
	} else {
		info->info.data.apci = (tpdu[0] << 2 & 12) | (tpdu[1] >> 6 & 3);
		info->info.data.length = length - 1;
		info->info.data.payload = tpdu + 1;
	}

	return true;
}

void knx_tpdu_generate(uint8_t* tpdu, const knx_tpdu* info) {
	tpdu[0] = (info->tpci & 3) << 6 | (info->seq_number & 15) << 2;

	switch (info->tpci) {
		case KNX_TPCI_UNNUMBERED_DATA:
		case KNX_TPCI_NUMBERED_DATA:
			memcpy(tpdu + 1, info->info.data.payload, info->info.data.length);
			tpdu[1] &= 63;
			tpdu[1] |= (info->info.data.apci & 15) << 6;
			break;

		case KNX_TPCI_UNNUMBERED_CONTROL:
		case KNX_TPCI_NUMBERED_CONTROL:
			tpdu[1] |= info->info.control & 3;
			break;
	}
}

// inline static bool knx_tpdu_is_data(const knx_tpdu* info) {
// 	return
// 		info->tpci == KNX_TPCI_UNNUMBERED_DATA ||
// 		info->tpci == KNX_TPCI_NUMBERED_DATA;
// }

// bool knx_tpdu_interpret(const uint8_t* tpdu, size_t length, knx_dpt type, void* value) {
// 	knx_tpdu info;
// 	if (!knx_tpdu_parse(tpdu, length, &info) || !knx_tpdu_is_data(&info))
// 		return false;

// 	return knx_dpt_from_apdu(info.info.data.payload, info.info.data.length, type, value);
// }

// void knx_tpdu_generate(uint8_t* tpdu, knx_apci apci, knx_dpt type, const void* value) {
// 	tpdu[0] = apci >> 2 & 3;
// 	tpdu[1] = (apci & 3) << 6;
// 	knx_dpt_to_apdu(tpdu + 1, type, value);
// }

// uint8_t* knx_tpdu_generate_(size_t* length, knx_apci apci, knx_dpt type, const void* value) {
// 	uint8_t* tpdu = newa(uint8_t, *length = 1 + knx_dpt_size(type));

// 	if (tpdu) {
// 		knx_tpdu_generate(tpdu, apci, type, value);
// 	}

// 	return tpdu;
// }
