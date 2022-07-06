#include "macros.h"
#include "corth.h"

#include <string.h>

int main(int argc, char **argv)
{
    if(strlen(argv[argc - 1]) < 6 || strcmp(argv[argc - 1] + strlen(argv[argc - 1]) - 6, ".corth"))
    {
        ERROR("invalid input file");
    }
    if(argc == 2)
    {
        compile(argv[argc - 1]);
        #if DEBUG
            printf("[DEBUG] Debug info:\n");
            print_mem();
            printf("[DEBUG] End debug info.\n");
        #else
            check_mem();
        #endif
        
        return 0;
    }
    ERROR("error");
    return 1;
}