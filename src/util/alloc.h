/* KNX Client Library
 * A library which provides the means to communicate with several
 * KNX-related devices or services.
 *
 * Copyright (C) 2014-2015, Ole Krüger <ole@vprsm.de>
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

#ifndef KNXPROTO_UTIL_ALLOC_H_
#define KNXPROTO_UTIL_ALLOC_H_

#include <malloc.h>

#ifdef DEBUG

#include "log.h"

inline static void* __debug_malloc(const char* file, size_t line, size_t n) {
	void* a = malloc(n);
	if (!a) knx_log_commit_raw(LOG_ERROR, file, line, "Failed to allocate %zu bytes", n);
	return a;
}

inline static void* __debug_realloc(const char* file, size_t line, void* p, size_t n) {
	void* a = realloc(p, n);
	if (!a) knx_log_commit_raw(LOG_ERROR, file, line, "Failed to reallocate %zu bytes", n);
	return a;
}

#define new(t) ((t*) __debug_malloc(__FILE__, __LINE__, sizeof(t)))
#define newa(t, n) ((t*) __debug_malloc(__FILE__, __LINE__, sizeof(t) * (n)))
#define renewa(p, t, n) ((t*) __debug_realloc(__FILE__, __LINE__, p, sizeof(t) * (n)))

#else /* DEBUG */

/**
 * Allocate a new instance of `t`.
 */
#define new(t) ((t*) malloc(sizeof(t)))

/**
 * Allocate an array of `n` instances of `t`.
 */
#define newa(t, n) ((t*) malloc(sizeof(t) * (n)))

/**
 * Reallocate an array of `n` instances of `t`.
 */
#define renewa(p, t, n) ((t*) realloc(p, sizeof(t) * (n)))

#endif /* DEBUG */

/**
 * Define an anonymous array of `t`s.
 * This uses a C99 feature and can only be used to pass them to functions.
 */
#define anona(t, ...) ((t[]) {__VA_ARGS__})

/**
 * Define an anonymous pointer to an instance of `t`.
 */
#define anon(t, ...) ((t[]) {{__VA_ARGS__}})

#endif
