#ifndef HPKNXD_LOG_H
#define HPKNXD_LOG_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Log Level
 */
typedef enum {
	LOG_INFO = 0,
	LOG_WARN = 1,
	LOG_ERROR = 2
} log_level;

#ifdef DEBUG

/**
 * Start logging
 */
bool log_setup(const char* path, log_level level);

/**
 * Raw log entry
 */
void log_commit_raw(log_level level, const char* file, size_t line, const char* format, ...);

#else

#define log_setup(...) 1
#define log_commit_raw(...)

#endif

/**
 * Log a information
 */
#define log_info(fmt, ...) log_commit_raw(LOG_INFO, __FILE__, __LINE__, (fmt), __VA_ARGS__)

/**
 * Log a warning
 */
#define log_warn(fmt, ...) log_commit_raw(LOG_WARN, __FILE__, __LINE__, (fmt), __VA_ARGS__)

/**
 * Log an error
 */
#define log_error(fmt, ...) log_commit_raw(LOG_ERROR, __FILE__, __LINE__, (fmt), __VA_ARGS__)

#endif
