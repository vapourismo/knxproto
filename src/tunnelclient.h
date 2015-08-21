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
 * Tunnel Connection
 */
struct _knx_tunnel_client {
	/**
	 * Socket used for communication
	 */
	int sock;

	/**
	 * Current connection state
	 */
	knx_tunnel_state state;

	/**
	 * Targeted KNXnet/IP gateway
	 */
	ip4addr gateway;

	/**
	 * Communication channel
	 */
	uint8_t channel;

	/**
	 * Host information (as seen from the gateway)
	 */
	knx_host_info host_info;

	/**
	 * Local package sequence counter
	 */
	uint8_t seq_number;

	/**
	 * Invoked when receiving a tunnel request
	 */
	knx_tunnel_recv_cb recv_cb;

	/**
	 * Data for the receive callback
	 */
	void* recv_data;

	/**
	 * Invoked when the connection state changes
	 */
	knx_tunnel_state_cb state_cb;

	/**
	 * Data for the state-change callback
	 */
	void* state_data;
};

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

#endif
