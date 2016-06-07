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

#ifndef KNXPROTO_PROTO_DESCRES_H_
#define KNXPROTO_PROTO_DESCRES_H_

#include "../util/address.h"

#include <arpa/inet.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
	/**
	 * Service type/family
	 */
	uint8_t family;

	/**
	 * Service version
	 */
	uint8_t version;
} knx_description_service;

/**
 * Description Response
 */
typedef struct {
	/**
	 * KNX medium
	 */
	uint8_t medium;

	/**
	 * Connection status
	 */
	uint8_t status;

	/**
	 * Individual address
	 */
	knx_addr address;

	/**
	 * Installation ID
	 */
	uint16_t id;

	/**
	 * Serial number
	 */
	uint8_t serial[6];

	/**
	 * Multicast address
	 */
	in_addr_t multicast_address;

	/**
	 * MAC address
	 */
	uint8_t mac_address[6];

	/**
	 * Name
	 */
	char name[30];

	/**
	 * Number of services
	 */
	size_t num_services;

	/**
	 * Services array
	 */
	knx_description_service* services;
} knx_description_response;

/**
 * Parse a raw description response.
 *
 * \note You have to free the `services` array using `knx_description_response_free_services`.
 * \param message        Raw description response
 * \param message_length Number of bytes in `message`
 * \param res            Output description response
 */
bool knx_description_response_parse(
	const uint8_t*            message,
	size_t                    message_length,
	knx_description_response* res
);

/**
 * Free the dynamically allocated `services` array.
 */
void knx_description_response_free_services(knx_description_response* res);

/**
 * Description response size
 */
inline static
size_t knx_description_response_size(const knx_description_response* res) {
	return 56 + 2 * res->num_services;
}

#endif
