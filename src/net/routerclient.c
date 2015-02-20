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

#include "../util/log.h"
#include "../util/alloc.h"

#include <sys/socket.h>
#include <netinet/in.h>

void* knx_router_worker_thread(void* data) {
	knx_router_client* client = data;

	// Join multicast group
	struct ip_mreq req = {
		.imr_interface = {INADDR_ANY},
		.imr_multiaddr = client->router.sin_addr,
	};

	if (setsockopt(client->sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &req, sizeof(req)) < 0) {
		log_error("Could not join multicast group");
		return NULL;
	}

	setsockopt(client->sock, IPPROTO_IP, IP_MULTICAST_LOOP, anona(int, 0), sizeof(int));

	// Set state
	client->state = KNX_ROUTER_LISTENING;

	while (client->state == KNX_ROUTER_LISTENING) {
		// TODO: Receive and queue message
	}

	// Leave multicast group
	if (setsockopt(client->sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, &req, sizeof(req)) < 0)
		log_error("Could not leave multicast group");

	pthread_exit(NULL);
}

bool knx_router_connect(knx_router_client* client, int sock, const ip4addr* router) {
	if (sock < 0)
		return false;

	client->sock = sock;
	client->router = *router;

	return pthread_create(&client->worker, NULL, &knx_router_worker_thread, client) == 0;
}

void knx_router_disconnect(knx_router_client* client) {
	client->state = KNX_ROUTER_LISTENING;
	pthread_join(client->worker, NULL);
}
