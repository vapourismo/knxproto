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

#include <stdbool.h>
#include <ev.h>

/**
 * Tunnel Connection State
 */
typedef enum {
	KNX_TUNNEL_CONNECTING,
	KNX_TUNNEL_CONNECTED,
	KNX_TUNNEL_DISCONNECTED
} knx_tunnel_state;

struct _knx_tunnel_client;

/**
 * Receive callback
 */
typedef void (* knx_tunnel_recv_callback)(struct _knx_tunnel_client*, const knx_ldata*, void*);

/**
 * State change callback
 */
typedef void (* knx_tunnel_state_callback)(struct _knx_tunnel_client*, knx_tunnel_state, void*);

/**
 * Tunnel Connection
 */
typedef struct _knx_tunnel_client {
	int sock;
	ip4addr gateway;

	// Connection information
	knx_tunnel_state state;
	uint8_t channel;
	knx_host_info host_info;

	// Packet counter
	uint8_t seq_number;

	// Receive callback
	knx_tunnel_recv_callback recv_cb;
	void* recv_data;

	// State change callback
	knx_tunnel_state_callback state_cb;
	void* state_data;

	// Events
	struct ev_io ev_read;
	struct ev_timer ev_heartbeat;
} knx_tunnel_client;

/**
 * Connect to a gateway.
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
 * Process the KNX packet as if it has been received through the router socket.
 */
void knx_tunnel_process_packet(knx_tunnel_client* client, const knx_packet* pkg_in);

/**
 * Start processing packets within the event loop.
 */
void knx_tunnel_start(knx_tunnel_client* client, struct ev_loop* loop,
                      knx_tunnel_recv_callback cb, void* data);

/**
 * Stop processing packets.
 */
void knx_tunnel_stop(knx_tunnel_client* client, struct ev_loop* loop);

#endif
