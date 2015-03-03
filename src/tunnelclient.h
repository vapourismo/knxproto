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

#include "proto/knxnetip/knxnetip.h"
#include "proto/cemi/cemi.h"

#include "util/address.h"

#include <pthread.h>
#include <stdbool.h>
#include <time.h>

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
	uint8_t* message;
	size_t size;

	struct knx_tunnel_message* next;
} knx_tunnel_message;

/**
 * Tunnel Connection
 */
typedef struct {
	int sock;
	ip4addr gateway;
	pthread_t worker;

	size_t msg_queue_size;
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
} knx_tunnel_client;

/**
 * Connect to a gateway. This function returns `true` if a connection has
 * been established.
 */
bool knx_tunnel_connect(knx_tunnel_client* client, const char* hostname, in_port_t port);

/**
 * Disconnect from a gateway. This procedure also acts as a deconstructor.
 * It will not clear the queue for incoming packets.
 */
void knx_tunnel_disconnect(knx_tunnel_client* client);

/**
 * Send data which should be tunnelled through the gateway.
 * Returns true if the message has been successfully acknowledged by the gateway.
 * This function is blocking.
 */
bool knx_tunnel_send(knx_tunnel_client* client, const void* payload, uint16_t length);

/**
 * Send a L_Data request frame through the gateway.
 * The return value is inherited from `knx_tunnel_send`.
 */
bool knx_tunnel_send_ldata(knx_tunnel_client* client, const knx_ldata* ldata);

/**
 * Sent a L_Data request frame using the given destination group address and TPDU.
 * The return value is inherited from `knx_tunnel_send`.
 */
bool knx_tunnel_send_tpdu(knx_tunnel_client* client, knx_addr dest, const uint8_t* tpdu, size_t length);

/**
 * Retrieve an incoming message. The resulting buffer has to be deallocated using `free`.
 * In case of success, it will return the buffer size, otherwise a negative integer;
 */
ssize_t knx_tunnel_recv(knx_tunnel_client* client, uint8_t** buffer);

/**
 * Retrieve incoming a L_Data indication. You have to `free` the returned pointer manually.
 * Returning a NULL pointer indicates a failure.
 */
knx_ldata* knx_tunnel_recv_ldata(knx_tunnel_client* client);

/**
 * Remove every element from the queue.
 * You must not call this before the tunnel connection has ended.
 */
void knx_tunnel_clear_queue(knx_tunnel_client* client);

#endif
