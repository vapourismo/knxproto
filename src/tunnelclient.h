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
 *
 * \param client Tunnel client
 * \param ldata L_Data frame which has been received
 * \param user Callback-associated user data
 */
typedef void (* knx_tunnel_recv_cb)(knx_tunnel_client*, const knx_ldata*, void*);

/**
 * Invoked when the tunnel connection state changes.
 *
 * \param client Tunnel client
 * \param state New connection state
 * \param user Callback-associated user data
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
 * Initialize a tunnel client structure.
 *
 * \param sock Datagram socket to be used for communication
 * \param on_state State-change callback  (see `knx_tunnel_state_cb`)
 * \param state_data Data for state-change callback
 * \param on_recv Receive callback (see `knx_tunnel_recv_cb`)
 * \param recv_data Data for receive callback
 */
void knx_tunnel_init(knx_tunnel_client* client, int sock,
                     knx_tunnel_state_cb on_state, void* state_data,
                     knx_tunnel_recv_cb on_recv, void* recv_data);

/**
 * Send a connection request to the gateway.
 *
 * \param address Address or hostname of the gateway
 * \param port Gateway port (default is 3671)
 */
bool knx_tunnel_connect(knx_tunnel_client* client, const char* address, in_port_t port);

/**
 * Send a disconnect request.
 */
bool knx_tunnel_disconnect(knx_tunnel_client* client);

/**
 * Send a L_Data request.
 *
 * \see knx_ldata
 * \param ldata L_Data frame which shall be sent to the gateway
 */
bool knx_tunnel_send(knx_tunnel_client* client, const knx_ldata* ldata);

/**
 * Send `Group Value Write` command.
 *
 * \see proto/data.h
 * \param dest Group address
 * \param type Datapoint type
 * \param value Pointer to the associated value
 */
bool knx_tunnel_write_group(knx_tunnel_client* client, knx_addr dest,
                            knx_dpt type, const void* value);

/**
 * Send a heartbeat.
 */
bool knx_tunnel_send_heartbeat(knx_tunnel_client* client);

/**
 * Receive a packet and process it.
 */
bool knx_tunnel_process(knx_tunnel_client* client);

/**
 * Process the packet as if it has been received through the socket.
 *
 * \see knx_packet
 * \param pkg_in Packet to be processed
 */
bool knx_tunnel_process_packet(knx_tunnel_client* client, const knx_packet* pkg_in);

#endif
