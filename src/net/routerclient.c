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

#define KNX_ROUTER_QUEUE_SIZE_CAP 1073741824  // 1 MiB queue cap
#define KNX_ROUTER_READ_TIMEOUT 100000        // 100ms

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
		if (!dgramsock_ready(client->sock, 0, KNX_ROUTER_READ_TIMEOUT))
			continue;

		// Prefetch header and check if we want that packet
		ssize_t buffer_size = dgramsock_peek_knx(client->sock);
		if (buffer_size < 0) {
			// This is not a KNXnet/IP packet so we'll discard it
			recvfrom(client->sock, NULL, 0, 0, NULL, NULL);

			continue;
		}


		// Gather packet
		uint8_t buffer[buffer_size];
		ssize_t r = dgramsock_recv(client->sock, buffer, buffer_size, NULL, 0);

		knx_packet pkg_in;

		// Parse and check if the packet is a routing indication
		if (r > 0 && knx_parse(buffer, r, &pkg_in) && pkg_in.service == KNX_ROUTING_INDICATION) {
			// Push the message onto the incoming queue
			pthread_mutex_lock(&client->mutex);

			knx_router_message* msg = NULL;
			if (client->msg_queue_size + pkg_in.payload.routing_ind.size <= KNX_ROUTER_QUEUE_SIZE_CAP &&
			    (msg = new(knx_router_message)) &&
			    (msg->message = newa(uint8_t, pkg_in.payload.routing_ind.size))) {

				// Prepare element
				msg->next = NULL;
				msg->size = pkg_in.payload.routing_ind.size;
				memcpy(msg->message, pkg_in.payload.routing_ind.data, msg->size);

				// Insert element
				if (client->msg_head)
					client->msg_tail = client->msg_tail->next = msg;
				else
					client->msg_tail = client->msg_head = msg;

				client->msg_queue_size += msg->size + sizeof(knx_router_message);

				pthread_cond_signal(&client->cond);
			} else if (msg) {
				log_error("Allocating new queue element failed");
				free(msg);
			} else if (client->msg_queue_size + pkg_in.payload.routing_ind.size > KNX_ROUTER_QUEUE_SIZE_CAP) {
				log_info("Reached maximum queue size");
			}

			pthread_mutex_unlock(&client->mutex);
		}
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
