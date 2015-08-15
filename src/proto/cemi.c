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

#include "cemi.h"
#include "../util/log.h"

#include <string.h>

void knx_cemi_unpack_header(const uint8_t* buffer, knx_cemi_service* service, uint8_t* info_length) {
	if (service)
		*service = buffer[0];

	if (info_length)
		*info_length = buffer[1];

	knx_log_debug("cemi: service = %i, info_length = %i", buffer[0], buffer[1]);
}

bool knx_cemi_parse(const uint8_t* message, size_t length, knx_cemi* frame) {
	if (length < KNX_CEMI_HEADER_SIZE) {
		knx_log_error("Insufficient message length %zu", length);
		return false;
	}

	// Unpack header and store additional info details
	knx_cemi_unpack_header(message, &frame->service, &frame->add_info_length);

	// Does the additional information exceed the frame?
	if (KNX_CEMI_HEADER_SIZE + ((size_t) frame->add_info_length) > length)
		return false;

	frame->add_info = message + KNX_CEMI_HEADER_SIZE;
	message += KNX_CEMI_HEADER_SIZE + frame->add_info_length;
	length -= KNX_CEMI_HEADER_SIZE + frame->add_info_length;

	switch (frame->service) {
		case KNX_CEMI_LDATA_IND:
		case KNX_CEMI_LDATA_REQ:
		case KNX_CEMI_LDATA_CON:
			return knx_ldata_parse(message, length, &frame->payload.ldata);

		default:
			knx_log_error("Unsupported CEMI service %02X", frame->service);
			return false;
	}
}

bool knx_cemi_generate(uint8_t* buffer, const knx_cemi* frame) {
	buffer[0] = frame->service;
	buffer[1] = frame->add_info_length;

	if (frame->add_info_length > 0 && frame->add_info)
		memcpy(buffer + KNX_CEMI_HEADER_SIZE, frame->add_info, frame->add_info_length);

	// Calculate buffer offset
	buffer += KNX_CEMI_HEADER_SIZE + frame->add_info_length;

	switch (frame->service) {
		case KNX_CEMI_LDATA_IND:
		case KNX_CEMI_LDATA_REQ:
		case KNX_CEMI_LDATA_CON:
			return knx_ldata_generate(buffer, &frame->payload.ldata);

		default:
			knx_log_error("Unsupported CEMI service %02X", frame->service);
			return false;
	}
}

size_t knx_cemi_size(const knx_cemi* frame) {
	switch (frame->service) {
		case KNX_CEMI_LDATA_IND:
		case KNX_CEMI_LDATA_REQ:
		case KNX_CEMI_LDATA_CON:
			return
				KNX_CEMI_HEADER_SIZE +
				frame->add_info_length +
				knx_ldata_size(&frame->payload.ldata);

		default:
			knx_log_error("Unsupported CEMI service %02X", frame->service);
			return 0;
	}
}
