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

#include "../util/address.h"

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
 * Router state
 */
typedef enum {
	KNX_ROUTER_INACTIVE,
	KNX_ROUTER_LISTENING
} knx_router_state;

/**
 *  Router Client
 */
typedef struct {
	int sock;
	ip4addr router;

	pthread_t worker;

	size_t msg_queue_size;
	knx_router_message* msg_head;
	knx_router_message* msg_tail;

	pthread_mutex_t mutex;
	pthread_cond_t cond;

	knx_router_state state;
} knx_router_client;

/**
 * Connect to a router multicast group.
 */
bool knx_router_connect(knx_router_client* client, int sock, const ip4addr* router);

/**
 * Disconnect from router multicast group.
 */
void knx_router_disconnect(knx_router_client* client);

/**
 * Retrieve an incoming message.
 */
ssize_t knx_router_recv(knx_router_client* client, uint8_t** buffer);

/**
 * Clear the message queue.
 */
void knx_router_clear_queue(knx_router_client* client);


#endif
