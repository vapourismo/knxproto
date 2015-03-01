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

#ifndef KNXCLIENT_NET_ROUTERCLIENT_H
#define KNXCLIENT_NET_ROUTERCLIENT_H

#include "proto/cemi/cemi.h"

#include "util/address.h"

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

/**
 *  Router Client
 */
typedef struct {
	int sock;
	ip4addr router;

	struct ip_mreq mreq;
} knx_router_client;

/**
 * Join the router's multicast group.
 */
bool knx_router_connect(knx_router_client* client, const ip4addr* router);

/**
 * Leave the router's multicast group.
 */
bool knx_router_disconnect(const knx_router_client* client);

/**
 * Retrieve an incoming message. Returns a positive integer on success.
 * The resulting buffer has to be freed.
 */
ssize_t knx_router_recv(const knx_router_client* client, uint8_t** buffer, bool block);

/**
 * Retrieve incoming an L_Data indication. You have to free the returned pointer manually.
 */
knx_ldata* knx_router_recv_ldata(const knx_router_client* client, bool block);

/**
 * Send a message.
 */
bool knx_router_send(const knx_router_client* client, const uint8_t* payload, uint16_t length);

/**
 * Send a L_Data frame.
 */
bool knx_router_send_ldata(const knx_router_client* client, const knx_ldata* ldata);

#endif
