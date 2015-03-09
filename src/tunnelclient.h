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

#ifndef KNXCLIENT_NET_TUNNELCLIENT_H
#define KNXCLIENT_NET_TUNNELCLIENT_H

#include "proto/knxnetip.h"
#include "proto/cemi.h"

#include "util/address.h"

#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <event2/event.h>

/**
 * Tunnel Connection State
 */
typedef enum {
	KNX_TUNNEL_CONNECTING,
	KNX_TUNNEL_CONNECTED,
	KNX_TUNNEL_DISCONNECTED
} knx_tunnel_state;

/**
 * Message from the gateway
 */
typedef struct knx_tunnel_message {
	knx_ldata* ldata;
	struct knx_tunnel_message* next;
} knx_tunnel_message;

/**
 * Tunnel Connection
 */
typedef struct {
	int sock;
	ip4addr gateway;
	pthread_t worker;

	knx_tunnel_message* msg_head;
	knx_tunnel_message* msg_tail;

	pthread_mutex_t mutex;
	pthread_cond_t cond;

	knx_tunnel_state state;

	pthread_mutex_t send_mutex;

	uint8_t seq_number;
	uint8_t ack_seq_number;

	uint8_t channel;
	knx_host_info host_info;

	bool heartbeat;

	struct event_base* ev_manifest;
	struct event* ev_read;
	struct event* ev_heartbeat;
} knx_tunnel_client;

/**
 * Connect to a gateway. This function returns `true` if a connection has
 * been established.
 */
bool knx_tunnel_connect(knx_tunnel_client* client, const char* hostname, in_port_t port);

/**
 * Disconnect from the gateway if a connection is active, and free resources.
 * You must not invoke this method twice or on connections that did not
 * connect successfully, because this would lead to a double free of resources.
 */
void knx_tunnel_disconnect(knx_tunnel_client* client);

/**
 * Send a L_Data request frame through the gateway.
 * Returns `true` if the message has been successfully acknowledged.
 */
bool knx_tunnel_send(knx_tunnel_client* client, const knx_ldata* ldata);

/**
 * Send `Group Value Write` command.
 */
bool knx_tunnel_write_group(knx_tunnel_client* client, knx_addr dest,
                            knx_dpt type, const void* value);

/**
 * Retrieve incoming a L_Data indication. You have to `free` the returned pointer manually.
 * Returning a NULL pointer indicates a failure.
 */
knx_ldata* knx_tunnel_recv(knx_tunnel_client* client, bool block);

#endif
