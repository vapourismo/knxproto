#ifndef HPKNXD_MESSAGEBUILDER_H
#define HPKNXD_MESSAGEBUILDER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "alloc.h"

/**
 * Message Builder
 */
typedef struct {
	uint8_t* buffer;
	size_t used, max;
} msgbuilder;

/**
 * Allocate and initialize.
 */
msgbuilder* msgbuilder_new(size_t cap);

/**
 * Initialize and allocate buffer with the given capacity `cap`.
 */
bool msgbuilder_init(msgbuilder* mb, size_t cap);

/**
 * Free the underlying buffer.
 */
inline void msgbuilder_free_buffer(const msgbuilder* mb) {
	if (mb->buffer) free(mb->buffer);
}

/**
 * Free the entire structure including the buffer.
 */
inline void msgbuilder_free(msgbuilder* mb) {
	msgbuilder_free_buffer(mb);
	free(mb);
}

/**
 * Ensure space for `num` more elements.
 */
bool msgbuilder_reserve(msgbuilder* mb, size_t num);

#endif
