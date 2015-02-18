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

#include "header.h"

#include <arpa/inet.h>

// Header:
//   Octet 0:   Header length
//   Octet 1:   Protocol version
//   Octet 2-3: Service number
//   Octet 4-5: Packet length including header size

bool knx_generate_header(uint8_t* buffer, knx_service srv, uint16_t length) {
	// Since the protocol specifies the payload length
	// to be a 16-bit unsigned integer, we have to make
	// sure the given length + header size do not exceed
	// the uint16_t bounds.
	if (length > UINT16_MAX - KNX_HEADER_SIZE)
		return false;

	// This preamble will always be there,
	// unless the underlying KNXnet/IP version changes.
	buffer[0] = KNX_HEADER_SIZE;
	buffer[1] = 16;

	// Service description
	buffer[2] = srv >> 8 & 0xFF;
	buffer[3] = srv & 0xFF;

	// Entire packet size
	length += KNX_HEADER_SIZE;
	buffer[4] = length >> 8 & 0xFF;
	buffer[5] = length & 0xFF;

	return true;
}
