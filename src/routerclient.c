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

#include "routerclient.h"

#include "proto/knxnetip/knxnetip.h"

#include "util/log.h"
#include "util/alloc.h"
#include "util/sockutils.h"

#include <sys/socket.h>
#include <netinet/in.h>

bool knx_router_connect(knx_router_client* client, const ip4addr* router) {
	ip4addr local = *router;
	local.sin_addr.s_addr = INADDR_ANY;

	if ((client->sock = dgramsock_create(&local, true)) < 0) {
		log_error("Failed to create socket");
		return false;
	}

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
	bool r = setsockopt(client->sock, IPPROTO_IP,
	                    IP_DROP_MEMBERSHIP, &client->mreq, sizeof(client->mreq)) == 0;
	close(client->sock);
	return r;
}

ssize_t knx_router_recv(const knx_router_client* client, uint8_t** result_buffer, bool block) {
	if (!block && !dgramsock_ready(client->sock, 0, 0))
		return -1;

	ssize_t buffer_size = dgramsock_peek_knx(client->sock);

	if (buffer_size < 0) {
		// Discard this packet
		recvfrom(client->sock, NULL, 0, 0, NULL, NULL);

		log_warn("Dequeued bogus message");

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

knx_ldata* knx_router_recv_ldata(const knx_router_client* client, bool block) {
	uint8_t* data;
	ssize_t size = knx_router_recv(client, &data, block);

	knx_cemi_frame cemi;

	if (size < 0)
		return NULL;

	if (!knx_cemi_parse(data, size, &cemi) || cemi.service != KNX_CEMI_LDATA_IND) {
		log_error("Failed to parse as L_Data indication frame");
		free(data);
		return NULL;
	}

	// Preserve TPDU
	uint8_t tpdu[cemi.payload.ldata.length];
	memcpy(tpdu, cemi.payload.ldata.tpdu, cemi.payload.ldata.length);

	// Reallocate buffer space to fit the L_Data + TPDU
	knx_ldata* ret = realloc(data, sizeof(knx_ldata) + sizeof(tpdu));

	if (!ret)
		return NULL;

	// Copy L_Data and setup TPDU
	*ret = cemi.payload.ldata;
	ret->tpdu = (const uint8_t*) (ret + 1);
	memcpy(ret + 1, tpdu, sizeof(tpdu));

	return ret;
}

bool knx_router_send(const knx_router_client* client, const uint8_t* payload, uint16_t length) {
	knx_routing_indication route_ind = {
		length,
		payload
	};

	return dgramsock_send_knx(client->sock, KNX_ROUTING_INDICATION, &route_ind, &client->router);
}

bool knx_router_send_ldata(const knx_router_client* client, const knx_ldata* ldata) {
	uint8_t buffer[knx_cemi_size(KNX_CEMI_LDATA_IND, ldata)];
	knx_cemi_generate_(buffer, KNX_CEMI_LDATA_IND, ldata);
	return knx_router_send(client, buffer, sizeof(buffer));
}

bool knx_router_send_tpdu(const knx_router_client* client, knx_addr dest, const uint8_t* tpdu, size_t length) {
	knx_ldata ldata = {
		.control1 = {KNX_LDATA_PRIO_LOW, true, true, true, false},
		.control2 = {KNX_LDATA_ADDR_GROUP, 7},
		.source = 0,
		.destination = dest,
		.tpdu = tpdu,
		.length = length
	};

	return knx_router_send_ldata(client, &ldata);
}
