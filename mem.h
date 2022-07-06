#ifndef MEM_H
#define MEM_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "macros.h"

static uint64_t max_allocated_aux = 0;
static uint64_t max_allocated = 0;
static uint64_t allocated = 0;
static uint64_t malloc_used = 0;
static uint64_t free_used = 0;

void *my_malloc(uint64_t size);
void *my_realloc(void *ptr, uint64_t size);
void my_free(void *ptr);
void print_mem();
void check_mem();
#endif /* MEM_H */