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

#include "proto/knxnetip.h"

#include "util/log.h"
#include "util/alloc.h"
#include "util/sockutils.h"

#include <sys/socket.h>
#include <netinet/in.h>

bool knx_router_connect(knx_router_client* client, const ip4addr* router) {
	if (router) {
		client->router = *router;
	} else {
		ip4addr_from_string(&client->router, "224.0.23.12", 3671);
	}

	ip4addr local = client->router;
	local.sin_addr.s_addr = INADDR_ANY;

	if ((client->sock = knx_dgramsock_create(&local, true)) < 0) {
		knx_log_error("Failed to create socket");
		return false;
	}

	// Join multicast group
	client->mreq.imr_interface.s_addr = INADDR_ANY;
	client->mreq.imr_multiaddr.s_addr = client->router.sin_addr.s_addr;

	if (setsockopt(client->sock, IPPROTO_IP,
	               IP_ADD_MEMBERSHIP, &client->mreq, sizeof(client->mreq)) < 0) {
		knx_log_error("Could not join multicast group");
		close(client->sock);
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

knx_ldata* knx_router_recv(const knx_router_client* client, bool block) {
	if (!block && !knx_dgramsock_ready(client->sock, 0, 0))
		return NULL;

	ssize_t buffer_size = knx_dgramsock_peek_knx(client->sock);

	if (buffer_size < 0) {
		// Discard this packet
		recvfrom(client->sock, NULL, 0, 0, NULL, NULL);

		knx_log_warn("Dequeued bogus message");

		// We have to rely on the compiler to perform tail-call optimization here,
		// otherwise this might turn out horribly.
		// Alternatively we could use a goto ...
		return knx_router_recv(client, block);
	}

	uint8_t buffer[buffer_size];
	knx_packet packet;

	if (knx_dgramsock_recv(client->sock, buffer, buffer_size, &packet, NULL, 0) &&
	    packet.service == KNX_ROUTING_INDICATION) {

		switch (packet.payload.routing_ind.data.service) {
			case KNX_CEMI_LDATA_IND:
			case KNX_CEMI_LDATA_CON:
				return knx_ldata_duplicate(&packet.payload.routing_ind.data.payload.ldata);

			default:
				knx_log_error("Unsupported CEMI service %02X",
				              packet.payload.routing_ind.data.service);
				return knx_router_recv(client, block);
		}
	} else {
		knx_log_error("Error during recv after peek, this should be technically impossible");
		return NULL;
	}
}

bool knx_router_send(const knx_router_client* client, const knx_ldata* ldata) {
	knx_routing_indication route_ind = {
		{
			KNX_CEMI_LDATA_IND,
			0,
			NULL,
			{ .ldata = *ldata }
		}
	};

	return knx_dgramsock_send(client->sock, KNX_ROUTING_INDICATION, &route_ind, &client->router);
}

bool knx_router_write_group(knx_router_client* client, knx_addr dest,
                            knx_dpt type, const void* value) {
	uint8_t buffer[knx_dpt_size(type)];
	knx_dpt_to_apdu(buffer, type, value);

	knx_ldata frame = {
		.control1 = {KNX_LDATA_PRIO_LOW, true, true, true, false},
		.control2 = {KNX_LDATA_ADDR_GROUP, 7},
		.source = 0,
		.destination = dest,
		.tpdu = {
			.tpci = KNX_TPCI_UNNUMBERED_DATA,
			.info = {
				.data = {
					.apci = KNX_APCI_GROUPVALUEWRITE,
					.payload = buffer,
					.length = sizeof(buffer)
				}
			}
		}
	};

	return knx_router_send(client, &frame);
}
