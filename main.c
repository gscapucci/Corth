#include "macros.h"
#include "corth.h"

#include <string.h>

int main(int argc, char **argv)
{
    if(strlen(argv[argc - 1]) < 6 || !strcmp(argv[argc - 1] + strlen(argv[argc - 1]) - 6, ".corth"))
    {
        ERROR("invalid input file");
    }
    if(argc == 2)
    {
        start(argv[1]);
        return 0;
    }
    if(argc == 3 && !strcmp(argv[1], "-com"))
    {
        UNIMPLEMENTED("compilation");
        //compile(argc[1]);
        return 0;
    }
    ERROR("error");
    return 1;
}