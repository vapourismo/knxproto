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

#include "descres.h"
#include "../util/alloc.h"

#include <string.h>

bool knx_description_response_parse(
	const uint8_t*            buffer,
	size_t                    length,
	knx_description_response* res
) {
	if (length < 56 || buffer[0] != 54 || buffer[1] != 1 || buffer[54] % 2 != 0 || buffer[55] != 2)
		return false;

	res->medium = buffer[2];
	res->status = buffer[3];
	res->address = buffer[4] << 8 | buffer[5];
	res->id = buffer[6] << 8 | buffer[7];

	memcpy(&res->serial, buffer + 8, 6);
	memcpy(&res->multicast_address, buffer + 14, 4);
	memcpy(&res->mac_address, buffer + 18, 6);

	memcpy(&res->name, buffer + 24, 29);
	res->name[29] = 0;

	res->num_services = buffer[54] / 2;

	if (res->num_services == 0) {
		res->services = NULL;
		return true;
	}

	buffer += 56;
	length -= 56;

	res->services = newa(knx_description_service, res->num_services);

	for (size_t i = 0; i < res->num_services && length >= 2; length -= 2, buffer += 2, i++) {
		res->services[i].family = buffer[0];
		res->services[i].version = buffer[1];
	}

	return true;
}

void knx_description_response_free_services(knx_description_response* res) {
	if (res->services == NULL || res->num_services == 0)
		return;

	free(res->services);
	res->num_services = 0;
}
