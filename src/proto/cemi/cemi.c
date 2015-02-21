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

#include "cemi.h"

#include <string.h>

void knx_cemi_unpack_header(const uint8_t* buffer, knx_cemi_service* service, uint8_t* info_length) {
	if (service)
		*service = buffer[0];

	if (info_length)
		*info_length = buffer[1];
}

bool knx_cemi_parse(const uint8_t* message, size_t length, knx_cemi_frame* frame) {
	if (length < KNX_CEMI_HEADER_SIZE)
		return false;

	knx_cemi_unpack_header(message, &frame->service, &frame->add_info_length);
	frame->add_info = message + KNX_CEMI_HEADER_SIZE;

	switch (frame->service) {
		case KNX_CEMI_LDATA_IND:
		case KNX_CEMI_LDATA_REQ:
			return false;

		default: return false;
	}
}

bool knx_generate_cemi(uint8_t* buffer, knx_cemi_service service,
                       uint8_t* add_info, uint8_t add_info_length, const void* payload) {
	buffer[0] = service;
	buffer[1] = add_info_length;

	if (add_info_length > 0)
		memcpy(buffer + KNX_CEMI_HEADER_SIZE, add_info, add_info_length);

	buffer += KNX_CEMI_HEADER_SIZE + add_info_length;

	switch (service) {
		case KNX_CEMI_LDATA_IND:
		case KNX_CEMI_LDATA_REQ:
			knx_generate_ldata(buffer, payload);
			return true;

		default: return false;
	}
}