#ifndef MACROS_H
#define MACROS_H

#include <stdio.h>
#include <stdlib.h>
    
#define HERE(...)\
            fprintf(stderr, "%s:%d: function %s " __VA_ARGS__ "\n", __FILE__, __LINE__, __func__)

#define ERROR(...)                              \
            HERE(__VA_ARGS__);                  \
            exit(1)

#define UNIMPLEMENTED(...)                          \
            HERE(__VA_ARGS__" not implemented yet");\
            exit(1)


#endif /* MACROS_H */