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

#include "msgbuilder.h"

#include <string.h>
#include <stdio.h>

msgbuilder* msgbuilder_new(size_t cap) {
	msgbuilder* mb = new(msgbuilder);

	if (mb)
		msgbuilder_init(mb, cap);

	return mb;
}

bool msgbuilder_init(msgbuilder* mb, size_t cap) {
	mb->buffer = cap > 0 ? newa(uint8_t, cap) : NULL;

	if (cap > 0 && !mb->buffer)
		return false;

	mb->used = 0;
	mb->max = cap;

	return true;
}

bool msgbuilder_reserve(msgbuilder* mb, size_t num) {
	if (mb->buffer == NULL)
		return msgbuilder_init(mb, num);

	// Has enough space?
	if (mb->max - mb->used >= num)
		return true;

	size_t new_cap = mb->used + num;

	// Save current buffer
	uint8_t tmp[mb->used];
	memcpy(tmp, mb->buffer, mb->used);

	// Reallocate new buffer
	uint8_t* new_buffer = renewa(mb->buffer, uint8_t, new_cap);

	if (!new_buffer)
		return false;

	// Need to copy old buffer to new location?
	if (new_buffer != mb->buffer) {
		memcpy(new_buffer, tmp, mb->used);
		mb->buffer = new_buffer;
	}

	mb->max = new_cap;

	return true;
}

bool msgbuilder_append(msgbuilder* mb,
                       const uint8_t* restrict source,
                       size_t length) {
	if (!msgbuilder_reserve(mb, length))
		return false;

	memcpy(mb->buffer + mb->used, source, length);
	mb->used += length;

	return true;
}
