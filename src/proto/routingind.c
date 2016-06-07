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

#include "routingind.h"

#include <string.h>

// Routing Indication:
//   Octet 0-n: Payload

bool knx_routing_indication_generate(uint8_t* buffer, const knx_routing_indication* ind) {
	return knx_cemi_generate(buffer, &ind->data);
}

bool knx_routing_indication_parse(
	const uint8_t*          message,
	size_t                  message_length,
	knx_routing_indication* ind
) {
	return knx_cemi_parse(message, message_length, &ind->data);
}
