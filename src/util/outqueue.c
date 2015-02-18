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

#include "outqueue.h"
#include "alloc.h"

#include <string.h>

bool knx_outqueue_init(knx_outqueue* queue) {
	queue->head = queue->tail = NULL;
	return pthread_mutex_init(&queue->lock, NULL) == 0;
}

void knx_outqueue_clear(knx_outqueue* queue) {
	pthread_mutex_lock(&queue->lock);
	struct knx_outqueue_elem* item = queue->head;

	while (item) {
		struct knx_outqueue_elem* free_me = item;
		item = item->next;
		free(free_me);
	}

	queue->head = queue->tail = NULL;
	pthread_mutex_unlock(&queue->lock);
}

void knx_outqueue_destroy(knx_outqueue* queue) {
	knx_outqueue_clear(queue);
	pthread_mutex_destroy(&queue->lock);
}

bool knx_outqueue_push(knx_outqueue* queue, knx_service service, const void* payload) {
	struct knx_outqueue_elem* elem = new(struct knx_outqueue_elem);

	if (!elem)
		return false;

	elem->service = service;
	elem->next = NULL;

	elem->length = knx_payload_size(service, payload) + KNX_HEADER_SIZE;
	elem->message = newa(uint8_t, elem->length);

	if (!knx_generate(elem->message, service, payload))
		return false;

	pthread_mutex_lock(&queue->lock);

	if (queue->head)
		queue->tail = queue->tail->next = elem;
	else
		queue->head = queue->tail = elem;

	pthread_mutex_unlock(&queue->lock);

	return true;
}

ssize_t knx_outqueue_pop(knx_outqueue* queue, uint8_t** buffer, knx_service* service) {
	if (!queue->head)
		return -1;

	pthread_mutex_lock(&queue->lock);

	if (buffer) *buffer = queue->head->message;
	if (service) *service = queue->head->service;
	size_t length = queue->head->length;

	struct knx_outqueue_elem* free_me = queue->head;
	queue->head = queue->head->next;
	free(free_me);

	pthread_mutex_unlock(&queue->lock);

	return length;
}
