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

#ifndef KNXCLIENT_NET_TUNNELCLIENT_H
#define KNXCLIENT_NET_TUNNELCLIENT_H

#include "../proto/knxnetip.h"

#include "../util/pkgqueue.h"
#include "../util/outqueue.h"
#include "../util/address.h"

#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>

/**
 * Tunnel Connection State
 */
typedef enum {
	KNX_TUNNEL_CONNECTING,
	KNX_TUNNEL_CONNECTED,
	KNX_TUNNEL_DISCONNECTING,
	KNX_TUNNEL_DISCONNECTED
} knx_tunnel_state;

/**
 * Tunnel Connection
 */
typedef struct {
	int sock;
	ip4addr gateway;

	pthread_t worker_thread;

	knx_pkgqueue incoming;
	knx_outqueue outgoing;

	pthread_mutex_t state_lock;
	pthread_cond_t state_signal;
	knx_tunnel_state state;

	bool established;
	uint8_t channel;
	knx_host_info host_info;
	time_t last_heartbeat;
} knx_tunnel_connection;

// TODO: Disallow `knx_tunnel_connection` to be allocated on the stack.
// A `knx_tunnel_connection` has to outlive it's worker thread.

/**
 * Connect to a gateway. This function returns `true` if the initialization
 * was successful.
 */
bool knx_tunnel_connect(knx_tunnel_connection* conn, const ip4addr* gateway);

/**
 * Disconnect from a gateway. If `wait_for_worker` is true, this function will
 * block until a disconnect response has been given by the gateway.
 */
void knx_tunnel_disconnect(knx_tunnel_connection* conn, bool wait_for_worker);

#endif
