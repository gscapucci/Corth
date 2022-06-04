#ifndef MEM_H
#define MEM_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "macros.h"

#define DEBUG 0

static size_t allocated = 0;
static size_t malloc_used = 0;
static size_t free_used = 0;

void *my_malloc(size_t size);
void *my_realloc(void *ptr, uint64_t size);
void my_free(void *ptr);
void print_mem();
#endif /* MEM_H */