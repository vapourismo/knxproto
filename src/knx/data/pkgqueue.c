#include "pkgqueue.h"
#include "../../alloc.h"

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

bool knx_pkgqueue_enqueue(knx_pkgqueue* queue, const knxnetip_packet* packet) {
	knx_pkgqueue_elem* elem = new(knx_pkgqueue_elem);

	elem->next = NULL;
	memcpy(&elem->packet, packet, sizeof(knxnetip_packet));

	pthread_mutex_lock(&queue->write_lock);

	if (queue->head)
		queue->tail = queue->tail->next = elem;
	else
		queue->head = queue->tail = elem;

	pthread_mutex_unlock(&queue->write_lock);

	return true;
}

bool knx_pkgqueue_dequeue(knx_pkgqueue* queue, knxnetip_packet* packet) {
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
