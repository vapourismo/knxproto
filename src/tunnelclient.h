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
#include "proto/ldata.h"
#include "proto/data.h"

#include <stdbool.h>
#include <ev.h>

/**
 * Tunnel Connection State
 */
typedef enum {
	/**
	 * Trying to connect
	 */
	KNX_TUNNEL_CONNECTING,

	/**
	 * Connection has been established
	 */
	KNX_TUNNEL_CONNECTED,

	/**
	 * No connection is active
	 */
	KNX_TUNNEL_DISCONNECTED
} knx_tunnel_state;

/**
 * Tunnel Connection
 */
typedef struct _knx_tunnel_client knx_tunnel_client;

/**
 * Invoked when receiving an L_Data frame.
 */
typedef void (* knx_tunnel_recv_cb)(knx_tunnel_client*, const knx_ldata*, void*);

/**
 * Invoked when the tunnel connection state changes.
 */
typedef void (* knx_tunnel_state_cb)(knx_tunnel_client*, knx_tunnel_state, void*);

/**
 * Create a new tunnel client. This function only allocates the necessary resources, it does not
 * perform any tunneling-related actions. On failure `NULL` is returned, otherwise a pointer to the
 * newly created tunnel client.
 */
knx_tunnel_client* knx_tunnel_new(knx_tunnel_state_cb on_state, void* state_data,
                                  knx_tunnel_recv_cb on_recv, void* recv_data);

/**
 * Destroys a tunnel client. You must not destroy the instance if the client is connected.
 */
void knx_tunnel_destroy(knx_tunnel_client* client);

/**
 * Send a connection request to the gateway.
 */
bool knx_tunnel_connect(knx_tunnel_client* client, const char* hostname, in_port_t port);

/**
 * Send a disconnect request.
 */
bool knx_tunnel_disconnect(knx_tunnel_client* client);

/**
 * Get the internal socket file descriptor.
 */
int knx_tunnel_get_socket(const knx_tunnel_client* client);

/**
 * Make the internal socket non-blocking.
 */
bool knx_tunnel_make_nonblocking(const knx_tunnel_client* client);

/**
 * Make the internal socket blocking.
 */
bool knx_tunnel_make_blocking(const knx_tunnel_client* client);

/**
 * Check if the internal socket is non-blocking.
 */
bool knx_tunnel_is_nonblocking(const knx_tunnel_client* client);

/**
 * Send a L_Data request.
 */
bool knx_tunnel_send(knx_tunnel_client* client, const knx_ldata* ldata);

/**
 * Send `Group Value Write` command.
 */
bool knx_tunnel_write_group(knx_tunnel_client* client, knx_addr dest,
                            knx_dpt type, const void* value);

/**
 * Send a heartbeat.
 */
bool knx_tunnel_send_heartbeat(knx_tunnel_client* client);

/**
 * Process one packet. If you are not using `knx_tunnel_start` to process packets, you must
 * invoke this function continuously in order to make the tunnel protocol logic work.
 */
bool knx_tunnel_process(knx_tunnel_client* client);

/**
 * Process the KNX packet as if it has been received through the socket.
 */
bool knx_tunnel_process_packet(knx_tunnel_client* client, const knx_packet* pkg_in);

/**
 * Start processing packets within the event loop. Note, this make the internal socket non-blocking.
 */
void knx_tunnel_start(knx_tunnel_client* client, struct ev_loop* loop);

/**
 * Stop processing packets.
 */
void knx_tunnel_stop(knx_tunnel_client* client, struct ev_loop* loop);

#endif
