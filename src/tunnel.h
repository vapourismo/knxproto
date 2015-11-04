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

#ifndef KNXPROTO_NET_TUNNELCLIENT_H_
#define KNXPROTO_NET_TUNNELCLIENT_H_

#include "proto/proto.h"
#include "proto/ldata.h"
#include "proto/data.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct _knx_tunnel knx_tunnel;

typedef void (* knx_tunnel_state_change_cb)(
	const knx_tunnel* tunnel,
	void*             data
);

typedef void (* knx_tunnel_send_cb)(
	const knx_tunnel* tunnel,
	void*             data,
	const uint8_t*    message,
	size_t            message_size
);

typedef void (* knx_tunnel_recv_cb)(
	const knx_tunnel* tunnel,
	void*             data,
	const knx_cemi*   frame
);

typedef void (* knx_tunnel_ack_cb)(
	const knx_tunnel* tunnel,
	void*             data,
	uint8_t           seq_number
);

/**
 * Contains the current state of a tunneling interaction.
 */
struct _knx_tunnel {
	enum {
		KNX_TUNNEL_CONNECTING,
		KNX_TUNNEL_CONNECTED,
		KNX_TUNNEL_DISCONNECTING,
		KNX_TUNNEL_DISCONNECTED
	} state;

	uint8_t channel;
	uint8_t seq_number;

	knx_tunnel_state_change_cb state_change;
	void* state_change_data;

	knx_tunnel_send_cb send_message;
	void* send_message_data;

	knx_tunnel_recv_cb handle_cemi;
	void* handle_cemi_data;

	knx_tunnel_ack_cb handle_ack;
	void* handle_ack_data;
};

/**
 * Initialize the `knx_tunnel` structure.
 */
void knx_tunnel_init(knx_tunnel* tunnel);

/**
 * Set the callback which will be used to send messages to the gateway.
 */
void knx_tunnel_set_send_handler(
	knx_tunnel*        tunnel,
	knx_tunnel_send_cb callback,
	void*              data
);

/**
 * Register a callback which will be invoke when the connection state changes.
 */
void knx_tunnel_set_state_change_handler(
	knx_tunnel*                tunnel,
	knx_tunnel_state_change_cb callback,
	void*                      data
);

/**
 * Register a callback which will be invoked each time a CEMI frame has been received.
 */
void knx_tunnel_set_recv_handler(
	knx_tunnel*        tunnel,
	knx_tunnel_recv_cb callback,
	void*              data
);

/**
 * Register a callback which will be invoked when a transmitted CEMI frame has been acknowledged.
 */
void knx_tunnel_set_ack_handler(
	knx_tunnel*       tunnel,
	knx_tunnel_ack_cb callback,
	void*             data
);

/**
 * Connect to the gateway.
 */
void knx_tunnel_connect(knx_tunnel* state);

/**
 * Process a message from the gateway.
 */
bool knx_tunnel_process(
	knx_tunnel*    tunnel,
	const uint8_t* message,
	size_t         message_size
);

/**
 * Send a CEMI frame to the gateway.
 * \returns Sequence number
 */
int16_t knx_tunnel_send(
	knx_tunnel*     tunnel,
	const knx_cemi* frame
);

/**
 * Send a CEMI frame to the gateway.
 */
bool knx_tunnel_resend(
	knx_tunnel*     tunnel,
	uint8_t         seq_number,
	const knx_cemi* frame
);

/**
 * Disconnect from the gateway.
 */
void knx_tunnel_disconnect(knx_tunnel* tunnel);

#endif
