#ifndef HPKNXD_ALLOC_H
#define HPKNXD_ALLOC_H

#include <malloc.h>

#define new(t) ((t*) malloc(sizeof(t)))
#define newa(t, n) ((t*) malloc(sizeof(t) * (n)))
#define renewa(p, t, n) ((t*) realloc(p, sizeof(t) * (n)))

#endif
