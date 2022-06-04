#include "mem.h"

typedef struct Memory Memory;
struct Memory
{
    uintptr_t loc;
    uint64_t size;
};

#if DEBUG
    #define MAX_NUMBER_OF_ALLOCATIONS 1024*1024
    Memory memory[MAX_NUMBER_OF_ALLOCATIONS] = {0};
#endif

void *my_malloc(uint64_t size)
{
    allocated += size;
    malloc_used++;
    #if DEBUG
        void *mem = malloc(size);
        for (uint64_t i = 0; i < MAX_NUMBER_OF_ALLOCATIONS; i++)
        {
            if(size == 8 && mem == (void *)0x5555566fa080)
            {
                printf("\n\nhere\n\n");
            }
            if(memory[i].loc == 0)
            {
                memory[i].loc = (uintptr_t)mem;
                memory[i].size = size;
                break;
            }
        }
        printf("allocated %zu bytes at %p\n", size, mem);
        return mem;
    #else
        return malloc(size);
    #endif
}

void *my_realloc(void *ptr, uint64_t size)
{
    #if DEBUG
        void *mem = realloc(ptr, size); 
        for (uint64_t i = 0; i < MAX_NUMBER_OF_ALLOCATIONS; i++)
        {
            if(memory[i].loc == (uintptr_t)ptr)
            {
                printf("realloc at %p(%zu bytes) to %p(%zu bytes)\n", ptr, memory[i].size, mem, size);
                memory[i].loc = (uintptr_t)mem;
                memory[i].size = size;
                break;
            }
        }
        return mem;
    #endif
    return realloc(ptr, size);
}

void my_free(void *ptr)
{
    free_used++;
    #if DEBUG
        for (uint64_t i = 0; i < MAX_NUMBER_OF_ALLOCATIONS; i++)
        {
            if(memory[i].loc == (uintptr_t)ptr)
            {
                printf("freed at %p(%zu bytes)\n", ptr, memory[i].size);
                memory[i].loc = 0;
                memory[i].size = 0;
                break;
            }
        }
    #endif
    free(ptr);
}

#if DEBUG
    void print_mem()
    {
        printf("mallocs: %zu\n", malloc_used);
        printf("frees: %zu\n", free_used);
        printf("diff: %zu\n", malloc_used - free_used);
        if(malloc_used - free_used != 0)
        {
            for (uint64_t i = 0; i < MAX_NUMBER_OF_ALLOCATIONS; i++)
            {
                if(memory[i].size != 0)
                {
                    fprintf(stderr, "unfreed %p(%zu bytes) i(%zu)\n", (void *)memory[i].loc, memory[i].size, i);
                }
            }
            ERROR("unfreed memory\n");
        }
        printf("total allocated: %zu(bytes)\n", allocated);
    }
#endif        

