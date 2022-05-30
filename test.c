#include <stdlib.h>
#include <stdio.h>

// #include "corth.h"

int main()
{
    char *ptr = malloc(1000);
    printf("%zu", sizeof(ptr));
    free(ptr);
    return 0;
}