#ifndef MACROS_H
#define MACROS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
    
#define HERE(...)\
            fprintf(stderr, "%s:%d: function %s " __VA_ARGS__ "\n", __FILE__, __LINE__, __func__)

#define ERROR(...)                              \
            HERE(__VA_ARGS__);                  \
            exit(1)

#define UNIMPLEMENTED(...)                          \
            HERE(__VA_ARGS__" not implemented yet");\
            exit(1)

#define START_TIMER(t) clock_t t = clock(), t##_end
#define END_TIMER(t) t##_end = clock()
#define PRINT_TIMER(t) printf("function %s took %lfs\n", __func__, ((double)(t##_end - t))/CLOCKS_PER_SEC);

#endif /* MACROS_H */