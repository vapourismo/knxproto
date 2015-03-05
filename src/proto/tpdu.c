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

bool knx_tpdu_info_parse(const uint8_t* tpdu, size_t length, knx_tpdu_info* info) {
	if (length == 0)
		return false;

	info->tpci = tpdu[0] >> 6 & 3;
	info->seq_number = tpdu[0] >> 2 & 15;

	if (info->tpci == KNX_TPCI_UNNUMBERED_CONTROL || info->tpci == KNX_TPCI_NUMBERED_CONTROL) {
		info->payload.control = tpdu[0] & 3;
	} else if (length < 2) {
		return false;
	} else {
		info->payload.apci = (tpdu[0] << 2 & 12)
		                   | (tpdu[1] >> 6 & 3);
	}

	return true;
}

bool knx_tpdu_has_data(const uint8_t* tpdu, size_t length, knx_apci* apci) {
	knx_tpdu_info info;
	if (!knx_tpdu_info_parse(tpdu, length, &info))
		return false;

	if (info.tpci != KNX_TPCI_UNNUMBERED_DATA && info.tpci != KNX_TPCI_NUMBERED_DATA)
		return false;

	if (apci)
		*apci = info.payload.apci;

	return true;
}

bool knx_tpdu_interpret(const uint8_t* tpdu, size_t length, knx_dpt type, void* value) {
	knx_tpdu_info info;

	if (length < 2 || !knx_tpdu_info_parse(tpdu, length, &info) ||
	    info.tpci == KNX_TPCI_NUMBERED_CONTROL || info.tpci == KNX_TPCI_UNNUMBERED_CONTROL)
			return false;

	return knx_dpt_from_apdu(tpdu + 1, length - 1, type, value);
}

void knx_tpdu_generate(uint8_t* tpdu, knx_apci apci, knx_dpt type, const void* value) {
	tpdu[0] = apci >> 2 & 3;
	tpdu[1] = (apci & 3) << 6;
	knx_dpt_to_apdu(tpdu + 1, type, value);
}
