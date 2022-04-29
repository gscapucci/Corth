#include "macros.h"
#include "corth.h"

#include <string.h>

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        ERROR("input a single file");
    }
    if(strlen(argv[1]) < 6 || strcmp(argv[1] + strlen(argv[1]) - 6, ".corth") != 0)
    {
        ERROR("invalid input file");
    }
    start(argv[1]);
    return 0;
}