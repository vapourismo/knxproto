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

#ifndef KNXCLIENT_UTIL_LOG_H
#define KNXCLIENT_UTIL_LOG_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Log Level
 */
typedef enum {
	LOG_DEBUG = 0,
	LOG_INFO = 1,
	LOG_WARN = 2,
	LOG_ERROR = 3
} knx_log_level;

/**
 * Start logging
 */
bool knx_log_setup(const char* path, knx_log_level level);

/**
 * Raw log entry
 */
void knx_log_commit_raw(knx_log_level level, const char* file, size_t line, const char* format, ...);

#ifdef DEBUG
	/**
	 * Log a debug message
	 */
	#define knx_log_debug(...) knx_log_commit_raw(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

	/**
	 * Log a information
	 */
	#define knx_log_info(...) knx_log_commit_raw(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)

	/**
	 * Log a warning
	 */
	#define knx_log_warn(...) knx_log_commit_raw(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)

	/**
	 * Log an error
	 */
	#define knx_log_error(...) knx_log_commit_raw(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#else
	#define knx_log_debug(...) ((void) NULL);
	#define knx_log_info(...) ((void) NULL);
	#define knx_log_warn(...) ((void) NULL);
	#define knx_log_error(...) ((void) NULL);
#endif

#endif
