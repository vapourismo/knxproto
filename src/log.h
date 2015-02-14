#ifndef HPKNXD_LOG_H
#define HPKNXD_LOG_H

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
} log_level;

/**
 * Start logging
 */
bool log_setup(const char* path, log_level level);

/**
 * Raw log entry
 */
void log_commit_raw(log_level level, const char* file, size_t line, const char* format, ...);

/**
 * Log a information
 */
#define log_info(...) log_commit_raw(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)

/**
 * Log a warning
 */
#define log_warn(...) log_commit_raw(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)

/**
 * Log an error
 */
#define log_error(...) log_commit_raw(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#ifdef DEBUG
	/**
	 * Log a debug message
	 */
	#define log_debug(...) log_commit_raw(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#else
	#define log_debug(...)
#endif

#endif
