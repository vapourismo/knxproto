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

#ifndef KNXCLIENT_NET_ROUTERCLIENT_H
#define KNXCLIENT_NET_ROUTERCLIENT_H

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

/**
 * Message from the router
 */
typedef struct knx_router_message {
	uint8_t* message;
	size_t size;

	struct knx_router_message* next;
} knx_router_message;

/**
 *  Router Client
 */
typedef struct {
	int sock;
	pthread_t worker;
} knx_router_client;

/**
 * Connect to a router multicast group.
 */
bool knx_router_connect(knx_router_client* client, int sock);

/**
 * Disconnect from router multicast group.
 */
bool knx_router_disconnect(knx_router_client* client);

#endif
