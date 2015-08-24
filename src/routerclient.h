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

#include "util/address.h"

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

/**
 * Router Client
 */
typedef struct {
	int sock;
	ip4addr router;

	struct ip_mreq mreq;
} knx_router_client;

/**
 * Join the router's multicast group. This function returns true,
 * if the socket has been created successfully and the mulicast group
 * has been joined.
 * It also tries to disable the mulicast loop back, but does not depend
 * on the success or failure of that operation.
 */
bool knx_router_connect(knx_router_client* client, const ip4addr* router);

/**
 * Leave the router's multicast group and close the underlying socket.
 * If the multicast group could not be left (due to a lack of membership) this
 * routine will return false, otherwise true.
 */
bool knx_router_disconnect(const knx_router_client* client);

/**
 * Retrieve incoming an L_Data indication. You have to `free` the returned pointer manually.
 * Returning a NULL pointer indicates a failure.
 */
knx_ldata* knx_router_recv(const knx_router_client* client, bool block);

/**
 * Send a L_Data indication frame.
 * Returns true if the frame has been properly transmitted.
 */
bool knx_router_send(const knx_router_client* client, const knx_ldata* ldata);

/**
 * Send a `Group Value Write` command.
 */
bool knx_router_write_group(knx_router_client* client, knx_addr dest,
                            knx_dpt type, const void* value);
#endif
