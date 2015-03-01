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

bool knx_tpdu_info_parse(const uint8_t* buffer, size_t length, knx_tpdu_info* info) {
	if (length == 0)
		return false;

	info->tpci = buffer[0] >> 6 & 3;
	info->seq_number = buffer[0] >> 2 & 15;

	if (info->tpci == KNX_TPCI_UNNUMBERED_CONTROL || info->tpci == KNX_TPCI_NUMBERED_CONTROL) {
		info->payload.control = buffer[0] & 3;
	} else if (length < 2) {
		return false;
	} else {
		info->payload.apci = (buffer[0] << 2 & 12)
		                   | (buffer[1] >> 6 & 3);
	}

	return true;
}
