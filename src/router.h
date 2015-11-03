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

#ifndef KNXPROTO_NET_ROUTERCLIENT_H_
#define KNXPROTO_NET_ROUTERCLIENT_H_

#include "proto/cemi.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct _knx_router knx_router;

typedef void (* knx_router_send_cb)(
	const knx_router* router,
	void*             data,
	const uint8_t*    message,
	size_t            message_size
);

typedef void (* knx_router_recv_cb)(
	const knx_router* router,
	void*             data,
	const knx_cemi*   frame
);

struct _knx_router {
	knx_router_send_cb send_message;
	void* send_message_data;

	knx_router_recv_cb handle_cemi;
	void* handle_cemi_data;
};

/**
 * Initialize the `knx_router` structure.
 */
void knx_router_init(knx_router* router);

/**
 * Set the callback which will be used to send messages.
 */
void knx_router_set_send_handler(
	knx_router*        router,
	knx_router_send_cb callback,
	void*              data
);

/**
 * Register a callback which will be invoked each time a CEMI frame has been received.
 */
void knx_router_set_recv_handler(
	knx_router*        router,
	knx_router_recv_cb callback,
	void*              data
);

/**
 * Process a message.
 */
bool knx_router_process(
	const knx_router* router,
	const uint8_t*    message,
	size_t            message_size
);

/**
 * Send a CEMI frame.
 */
void knx_router_send(
	const knx_router* router,
	const knx_cemi*   frame
);

#endif
