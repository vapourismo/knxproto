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

#ifndef KNXCLIENT_KNX_DATA_PKGQUEUE_H
#define KNXCLIENT_KNX_DATA_PKGQUEUE_H

#include "knx.h"

#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * Queue Element
 */
typedef struct _knx_pkgqueue_elem {
	knx_packet packet;
	struct _knx_pkgqueue_elem* next;
} knx_pkgqueue_elem;

/**
 * Packet Queue
 */
typedef struct {
	pthread_mutex_t write_lock;
	knx_pkgqueue_elem* head;
	knx_pkgqueue_elem* tail;
} knx_pkgqueue;

/**
 * Initialize the given packet queue.
 */
void knx_pkgqueue_init(knx_pkgqueue* queue);

/**
 * Remove every element from the queue.
 */
void knx_pkgqueue_clear(knx_pkgqueue* queue);

/**
 * Destroy the packet queue.
 */
void knx_pkgqueue_destroy(knx_pkgqueue* queue);

/**
 * Is the queue empty?
 */
inline bool knx_pkgqueue_empty(knx_pkgqueue* queue) {
	return queue->head == NULL;
}

/**
 * Add a packet at the back of a queue.
 */
bool knx_pkgqueue_enqueue(knx_pkgqueue* queue, const knx_packet* packet);

/**
 * Remove a packet from the head of a queue.
 */
bool knx_pkgqueue_dequeue(knx_pkgqueue* queue, knx_packet* packet);


#endif
