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

#ifndef KNXPROTO_PROTO_DCRES_H_
#define KNXPROTO_PROTO_DCRES_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Disconnect Response
 */
typedef struct {
	/**
	 * Communication channel
	 */
	uint8_t channel;

	/**
	 * Connection state (`0` is good)
	 */
	uint8_t status;
} knx_disconnect_response;

/**
 * Generate a raw disconnect response.
 *
 * \see KNX_DISCONNECT_RESPONSE_SIZE
 * \param buffer Output buffer, you have to make sure there is enough space
 * \param res    Input disconnect response
 */
void knx_disconnect_response_generate(uint8_t* buffer, const knx_disconnect_response* res);

/**
 * Parse a raw disconnect response.
 *
 * \param message        Raw disconnect response
 * \param message_length Number of bytes in `message`
 * \param res            Output disconnect response
 * \returns `true` if parsing was successful, otherwise `false`
 */
bool knx_disconnect_response_parse(
	const uint8_t*           message,
	size_t                   message_length,
	knx_disconnect_response* res
);

/**
 * Disconnect response size
 */
#define KNX_DISCONNECT_RESPONSE_SIZE 2

#endif
