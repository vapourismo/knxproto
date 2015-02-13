#ifndef HPKNXD_ALLOC_H
#define HPKNXD_ALLOC_H

#include <malloc.h>

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

/**
 * Define an anonymous array of `t`s.
 * This uses a C99 feature and can only be used to pass them to functions.
 */
#define anona(t, ...) ((t[]) {__VA_ARGS__})

#endif
