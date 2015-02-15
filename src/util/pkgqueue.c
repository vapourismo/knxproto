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

#include "pkgqueue.h"
#include "alloc.h"

#include <string.h>

void knx_pkgqueue_init(knx_pkgqueue* queue) {
	queue->head = queue->tail = NULL;
	pthread_mutex_init(&queue->write_lock, NULL);
}

void knx_pkgqueue_clear(knx_pkgqueue* queue) {
	pthread_mutex_lock(&queue->write_lock);
	knx_pkgqueue_elem* item = queue->head;

	while (item) {
		knx_pkgqueue_elem* free_me = item;
		item = item->next;
		free(free_me);
	}

	queue->head = queue->tail = NULL;
	pthread_mutex_unlock(&queue->write_lock);
}

void knx_pkgqueue_destroy(knx_pkgqueue* queue) {
	knx_pkgqueue_clear(queue);
	pthread_mutex_destroy(&queue->write_lock);
}

bool knx_pkgqueue_enqueue(knx_pkgqueue* queue, const knx_packet* packet) {
	knx_pkgqueue_elem* elem = new(knx_pkgqueue_elem);

	elem->next = NULL;
	memcpy(&elem->packet, packet, sizeof(knx_packet));

	pthread_mutex_lock(&queue->write_lock);

	if (queue->head)
		queue->tail = queue->tail->next = elem;
	else
		queue->head = queue->tail = elem;

	pthread_mutex_unlock(&queue->write_lock);

	return true;
}

bool knx_pkgqueue_dequeue(knx_pkgqueue* queue, knx_packet* packet) {
	bool success = false;
	pthread_mutex_lock(&queue->write_lock);

	if (queue->head) {
		*packet = queue->head->packet;

		knx_pkgqueue_elem* free_me = queue->head;
		queue->head = queue->head->next;

		free(free_me);

		success = true;
	}

	pthread_mutex_unlock(&queue->write_lock);
	return success;
}
