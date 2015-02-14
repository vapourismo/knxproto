#ifndef KNXCLIENT_KNX_DATA_OUTQUEUE_H
#define KNXCLIENT_KNX_DATA_OUTQUEUE_H

#include "../knx.h"
#include "../../msgbuilder.h"
#include <pthread.h>

/**
 *
 */
struct knx_outqueue_elem {
	knx_service service;
	uint8_t* message;
	size_t length;

	struct knx_outqueue_elem* next;
};

/**
 *
 */
typedef struct {
	pthread_mutex_t lock;

	struct knx_outqueue_elem* head;
	struct knx_outqueue_elem* tail;

	msgbuilder mb;
} knx_outqueue;

/**
 *
 */
bool knx_outqueue_init(knx_outqueue* queue);

/**
 *
 */
void knx_outqueue_clear(knx_outqueue* queue);

/**
 *
 */
void knx_outqueue_destroy(knx_outqueue* queue);

/**
 *
 */
bool knx_outqueue_push(knx_outqueue* queue, knx_service service, const void* payload);

/**
 *
 */
ssize_t knx_outqueue_pop(knx_outqueue* queue, uint8_t** buffer, knx_service* service);

#endif
