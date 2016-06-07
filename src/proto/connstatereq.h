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

#ifndef KNXPROTO_PROTO_CONNSTATEREQ_H_
#define KNXPROTO_PROTO_CONNSTATEREQ_H_

#include "hostinfo.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Connection State Request
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

	/**
	 * Control host information
	 */
	knx_host_info host;
} knx_connection_state_request;

/**
 * Generate a raw connection state request.
 *
 * \see KNX_CONNECTION_STATE_REQUEST_SIZE
 * \param buffer Output buffer, you have to make sure there is enough space
 * \param req    Input connection state request
 */
void knx_connection_state_request_generate(
	uint8_t*                            buffer,
	const knx_connection_state_request* req
);

/**
 * Parse a raw connection state request.
 *
 * \param message        Raw connection state request
 * \param message_length Number of bytes in `message`
 * \param req            Output connection state request
 * \returns `true` if parsing was successful, otherwise `false`
 */
bool knx_connection_state_request_parse(
	const uint8_t*                message,
	size_t                        message_length,
	knx_connection_state_request* req
);

/**
 * Connection state request size
 */
#define KNX_CONNECTION_STATE_REQUEST_SIZE (2 + KNX_HOST_INFO_SIZE)

#endif
