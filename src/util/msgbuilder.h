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

#ifndef KNXCLIENT_UTIL_MESSAGEBUILDER_H
#define KNXCLIENT_UTIL_MESSAGEBUILDER_H

#include <malloc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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
inline void msgbuilder_destroy(msgbuilder* mb) {
	if (mb->buffer) {
		free(mb->buffer);
		mb->buffer = NULL;
	}

	mb->used = mb->max = 0;
}

/**
 * Free the entire structure including the buffer.
 */
inline void msgbuilder_free(msgbuilder* mb) {
	if (mb->buffer)
		free(mb->buffer);

	free(mb);
}

/**
 * Ensure space for `num` more elements.
 */
bool msgbuilder_reserve(msgbuilder* mb, size_t num);

/**
 * Append data.
 */
bool msgbuilder_append(msgbuilder* mb,
                       const uint8_t* restrict source,
                       size_t length);

/**
 * Append data from another `msgbuilder`.
 */
inline bool msgbuilder_append_mb(msgbuilder* mb, const msgbuilder* rhs) {
	return msgbuilder_append(mb, rhs->buffer, rhs->used);
}

/**
 * Append a single byte.
 */
inline bool msgbuilder_append_single(msgbuilder* mb, uint8_t val) {
	if (!msgbuilder_reserve(mb, 1))
		return false;

	mb->buffer[mb->used++] = val;
	return true;
}

/**
 * Retrieve the underlying buffer and reset the message builder.
 */
inline size_t msgbuilder_move(msgbuilder* mb, uint8_t** buffer) {
	*buffer = mb->buffer;
	size_t len = mb->used;

	mb->buffer = NULL;
	mb->used = mb->max = 0;

	return len;
}

#endif
