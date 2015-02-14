#include "outqueue.h"
#include "../../alloc.h"

#include <string.h>

bool knx_outqueue_init(knx_outqueue* queue) {
	queue->head = queue->tail = NULL;
	pthread_mutex_init(&queue->lock, NULL);

	if (!msgbuilder_init(&queue->mb, 0)) {
		free(queue);
		return false;
	}

	return true;
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
	msgbuilder_destroy(&queue->mb);
}

bool knx_outqueue_push(knx_outqueue* queue, knx_service service, const void* payload) {
	struct knx_outqueue_elem* elem = new(struct knx_outqueue_elem);

	if (!elem)
		return false;

	elem->service = service;
	elem->next = NULL;

	pthread_mutex_lock(&queue->lock);

	if (!knx_generate(&queue->mb, service, payload)) {
		pthread_mutex_unlock(&queue->lock);
		return false;
	}

	elem->length = msgbuilder_move(&queue->mb, &elem->message);

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
