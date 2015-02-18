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

#ifndef KNXCLIENT_UTIL_OUTQUEUE_H
#define KNXCLIENT_UTIL_OUTQUEUE_H

#include "../proto/knxnetip.h"

#include <pthread.h>
#include <stdbool.h>

/**
 * Outgoing Queue Element
 */
struct knx_outqueue_elem {
	knx_service service;
	uint8_t* message;
	size_t length;

	struct knx_outqueue_elem* next;
};

/**
 * Outgoing Queue
 */
typedef struct {
	pthread_mutex_t lock;

	struct knx_outqueue_elem* head;
	struct knx_outqueue_elem* tail;
} knx_outqueue;

/**
 * Initialize the queue
 */
bool knx_outqueue_init(knx_outqueue* queue);

/**
 * Remove all elements
 */
void knx_outqueue_clear(knx_outqueue* queue);

/**
 * Destroy the queue
 */
void knx_outqueue_destroy(knx_outqueue* queue);

/**
 * Push an element onto the queue
 */
bool knx_outqueue_push(knx_outqueue* queue, knx_service service, const void* payload);

/**
 * Remove an element from the queue
 */
ssize_t knx_outqueue_pop(knx_outqueue* queue, uint8_t** buffer, knx_service* service);

/**
 * Check if the queue is empty
 */
inline bool knx_outqueue_empty(knx_outqueue* queue) {
	return queue->head == NULL;
}

#endif
