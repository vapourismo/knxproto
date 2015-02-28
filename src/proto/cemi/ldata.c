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

#include "ldata.h"
#include "../../util/log.h"

bool knx_ldata_generate(uint8_t* buffer, const knx_ldata* req) {
	if (req->length == 0 || req->length > UINT8_MAX + 1)
		return false;

	*buffer++ = (req->length <= 16) << 7                    // Standard Frame
	          | (~req->control1.repeat & 1) << 5            // Repeat
	          | (~req->control1.system_broadcast & 1) << 4  // System Broadcast
	          | (req->control1.priority & 3) << 2           // Priority
	          | (req->control1.request_ack & 1) << 1        // Want ACK?
	          | (req->control1.error & 1);                  // Error

	*buffer++ = (req->control2.address_type & 1) << 7       // Address Type
	          | (req->control2.hops & 7) << 4;              // Hop Count

	*buffer++ = req->source >> 8 & 0xFF;
	*buffer++ = req->source & 0xFF;

	*buffer++ = req->destination >> 8 & 0xFF;
	*buffer++ = req->destination & 0xFF;

	*buffer++ = req->length - 1;
	memcpy(buffer, req->tpdu, req->length);

	return true;
}

bool knx_ldata_parse(const uint8_t* buffer, size_t length, knx_ldata* out) {
	// Check for length and standard frame
	if (length < 8 || (buffer[1] & 15)) {
		log_debug("Insufficient length or unknown frame format");
		return false;
	}

	out->control1.repeat = !(buffer[0] >> 5 & 1);
	out->control1.system_broadcast = !(buffer[0] >> 4 & 1);
	out->control1.priority = buffer[0] >> 2 & 3;
	out->control1.request_ack = buffer[0] >> 1 & 1;
	out->control1.error = buffer[0] & 1;

	out->control2.address_type = buffer[1] >> 7 & 1;
	out->control2.hops = buffer[1] >> 4 & 7;

	out->source = buffer[2] << 8 | buffer[3];
	out->destination = buffer[4] << 8 | buffer[5];

	out->length = buffer[6] + 1;
	out->tpdu = buffer + 7;

	return out->length + 7 <= length;
}

size_t knx_ldata_size(const knx_ldata* req) {
	return 7 + req->length;
}
