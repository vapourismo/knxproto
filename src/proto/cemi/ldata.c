/* KNX Client Library
 * A library which provides the means to communicate with several
 * KNX-related devices or services.
 *
 * Copyright (C) 2014, Ole Krüger <ole@vprsm.de>
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

#include "ldata.h"

void knx_generate_ldata(uint8_t* buffer, const knx_ldata* req) {
	*buffer++ = req->control1;
	*buffer++ = req->control2;

	*buffer++ = req->source >> 8 & 0xFF;
	*buffer++ = req->source & 0xFF;

	*buffer++ = req->destination >> 8 & 0xFF;
	*buffer++ = req->destination & 0xFF;

	switch (req->tpci) {
		case KNX_LDATA_TPCI_NUMBERED_CONTROL:
			*buffer++ = 0;
			*buffer++ = (req->tpci & 3) << 6 |
			            (req->seq_number & 15) << 2 |
			            (req->payload.control & 3);
			break;

		case KNX_LDATA_TPCI_UNNUMBERED_CONTROL:
			*buffer++ = 0;
			*buffer++ = (req->tpci & 3) << 6 |
			            (req->payload.control & 3);
			break;

		case KNX_LDATA_TPCI_NUMBERED_DATA:
		case KNX_LDATA_TPCI_UNNUMBERED_DATA:
			*buffer++ = 1 + req->payload.apdu.length_over_6bit;

			// In case of an unnumbered data transmission we ignore the sequence number
			uint8_t seq_no = req->tpci == KNX_LDATA_TPCI_NUMBERED_DATA ? req->seq_number : 0;

			// TPCI + first 2 bits of APCI
			*buffer++ = (req->tpci & 3) << 6 |
			            (seq_no & 15) << 2 |
			            (req->payload.apdu.apci & 12) >> 2;

			// Last 2 bits of APCI + payload
			if (req->payload.apdu.length_over_6bit == 0) {
				*buffer++ = (req->payload.apdu.apci & 3) << 6 |
				            (req->payload.apdu.data ? *req->payload.apdu.data & 0x3F : 0);
			} else {
				*buffer++ = (req->payload.apdu.apci & 3) << 6;
				memcpy(buffer, req->payload.apdu.data, req->payload.apdu.length_over_6bit);
			}

			break;
	}
}

bool knx_parse_ldata(uint8_t* buffer, size_t length, knx_ldata* out) {
	if (length < 8)
		return false;

	out->control1 = buffer[0];
	out->control2 = buffer[1];

	out->source = buffer[2] << 8 | buffer[3];
	out->source = buffer[4] << 8 | buffer[5];

	out->tpci = buffer[7] >> 6 & 3;
	out->seq_number = 0;

	switch (out->tpci) {
		case KNX_LDATA_TPCI_NUMBERED_DATA:
			out->seq_number = buffer[7] >> 2 & 15;

		case KNX_LDATA_TPCI_UNNUMBERED_DATA:
			if (length != 8 + buffer[6] || !buffer[6]) {
				out->payload.apdu.length_over_6bit = 0;
				out->payload.apdu.data = NULL;
				return false;
			}

			out->payload.apdu.apci = (buffer[7] & 2) << 2 | (buffer[8] >> 6 & 2);

			buffer[8] &= 0x3F;
			out->payload.apdu.data = buffer + 8;
			out->payload.apdu.length_over_6bit = buffer[6] == 1 ? 1 : buffer[6] - 1;

			return true;

		case KNX_LDATA_TPCI_NUMBERED_CONTROL:
			out->seq_number = buffer[7] >> 2 & 15;

		case KNX_LDATA_TPCI_UNNUMBERED_CONTROL:
			out->payload.control = buffer[7] & 3;
			return true;

	}
}

size_t knx_ldata_size(const knx_ldata* req) {
	switch (req->tpci) {
		case KNX_LDATA_TPCI_NUMBERED_CONTROL:
		case KNX_LDATA_TPCI_UNNUMBERED_CONTROL:
			return 8;

		case KNX_LDATA_TPCI_NUMBERED_DATA:
		case KNX_LDATA_TPCI_UNNUMBERED_DATA:
			return 9 + req->payload.apdu.length_over_6bit;
	}
}
