#ifndef HPKNXD_KNX_DATA_PKGQUEUE_H
#define HPKNXD_KNX_DATA_PKGQUEUE_H

#include "../knx.h"

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
void knx_queue_init(knx_pkgqueue* queue);

/**
 * Remove every element from the queue.
 */
void knx_queue_clear(knx_pkgqueue* queue);

/**
 * Destroy the packet queue.
 */
void knx_queue_destroy(knx_pkgqueue* queue);

/**
 * Is the queue empty?
 */
inline bool knx_queue_empty(knx_pkgqueue* queue) {
	return queue->head == NULL;
}

/**
 * Add a packet at the back of a queue.
 */
bool knx_queue_enqueue(knx_pkgqueue* queue, const knx_packet* packet);

/**
 * Remove a packet from the head of a queue.
 */
bool knx_queue_dequeue(knx_pkgqueue* queue, knx_packet* packet);


#endif
