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

#include "routerclient.h"

#include "../proto/knxnetip.h"

#include "../util/log.h"
#include "../util/alloc.h"
#include "../util/sockutils.h"

#include <sys/socket.h>
#include <netinet/in.h>

bool knx_router_connect(knx_router_client* client, int sock, const ip4addr* router) {
	if (sock < 0)
		return false;

	client->sock = sock;
	client->router = *router;

	// Join multicast group
	client->mreq.imr_interface.s_addr = INADDR_ANY;
	client->mreq.imr_multiaddr.s_addr = router->sin_addr.s_addr;

	if (setsockopt(client->sock, IPPROTO_IP,
	               IP_ADD_MEMBERSHIP, &client->mreq, sizeof(client->mreq)) < 0) {
		log_error("Could not join multicast group");
		return false;
	}

	// Turn off multicast loopback
	setsockopt(client->sock, IPPROTO_IP, IP_MULTICAST_LOOP, anona(int, 0), sizeof(int));

	return true;
}

bool knx_router_disconnect(const knx_router_client* client) {
	return setsockopt(client->sock, IPPROTO_IP,
	                  IP_DROP_MEMBERSHIP, &client->mreq, sizeof(client->mreq)) == 0;
}

ssize_t knx_router_recv(const knx_router_client* client, uint8_t** result_buffer, bool block) {
	if (!block && !dgramsock_ready(client->sock, 0, 0))
		return -1;

	ssize_t buffer_size = dgramsock_peek_knx(client->sock);

	if (buffer_size < 0) {
		// Discard this packet
		recvfrom(client->sock, NULL, 0, 0, NULL, NULL);

		// We have to rely on the compiler to perform tail-call optimization here,
		// otherwise this might turn out horribly.
		// Alternatively we could use a goto ...
		return knx_router_recv(client, result_buffer, block);
	}

	uint8_t buffer[buffer_size];
	knx_packet packet;

	if (dgramsock_recv_knx(client->sock, buffer, buffer_size, &packet, NULL, 0) &&
	    packet.service == KNX_ROUTING_INDICATION) {
		uint8_t* payload = newa(uint8_t, packet.payload.routing_ind.size);

		if (!payload)
			return -1;

		memcpy(payload, packet.payload.routing_ind.data, packet.payload.routing_ind.size);

		*result_buffer = payload;
		return packet.payload.routing_ind.size;
	} else
		return -1;
}

bool knx_router_send(const knx_router_client* client, const uint8_t* payload, size_t length) {
	knx_routing_indication route_ind = {
		payload,
		length
	};

	return dgramsock_send_knx(client->sock, KNX_ROUTING_INDICATION, &route_ind, &client->router);
}
