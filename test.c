#include <stdlib.h>
#include <stdio.h>

#include "corth.h"

int main()
{
    size_t sum = sizeof(Stack) + sizeof(StringVec) + sizeof(MacroVec) + sizeof(DataTypeStack);
    printf("%zu\n", sizeof(Stack));
    printf("%zu\n", sizeof(StringVec));
    printf(" %zu\n", sizeof(MacroVec));
    printf(" %zu\n", sizeof(DataTypeStack));
    printf("-------\n"
           "%zu\n", (sum/1024)/1024);
    return 0;
}