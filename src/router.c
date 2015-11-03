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

#include "router.h"
#include "proto/knxnetip.h"

void knx_router_init(knx_router* router) {
	router->send_message = NULL;
	router->send_message_data = NULL;

	router->handle_cemi = NULL;
	router->handle_cemi_data = NULL;
}

void knx_router_set_send_handler(
	knx_router*        router,
	knx_router_send_cb callback,
	void*              data
) {
	router->send_message = callback;
	router->send_message_data = data;
}

void knx_router_set_recv_handler(
	knx_router*        router,
	knx_router_recv_cb callback,
	void*              data
) {
	router->handle_cemi = callback;
	router->handle_cemi_data = data;
}

bool knx_router_process(
	const knx_router* router,
	const uint8_t*    message,
	size_t            message_size
) {
	knx_packet packet;
	if (!knx_parse(message, message_size, &packet))
		return false;

	if (packet.service == KNX_ROUTING_INDICATION && router->handle_cemi)
		router->handle_cemi(router, router->handle_cemi_data, &packet.payload.routing_ind.data);

	return true;
}

void knx_router_send(
	const knx_router* router,
	const knx_cemi*   frame
) {
	if (!router->send_message)
		return;

	knx_routing_indication indication = {*frame};

	size_t message_size = knx_size(KNX_ROUTING_INDICATION, &indication);
	uint8_t message[message_size];
	knx_generate(message, KNX_ROUTING_INDICATION, &indication);

	router->send_message(router, router->send_message_data, message, message_size);
}
