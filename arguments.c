#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "arguments.h"


arguments_t* get_cli_arguments(int argc, char **argv) {
    int opt;
    arguments_t* parsed_args;

    // Reset getopt state for multiple calls
    optind = 1;
    
    parsed_args = malloc(sizeof(arguments_t));
    parsed_args->pattern = NULL;
    parsed_args->grep_command = NULL;

    while((opt = getopt(argc, argv, ":g:")) != -1) {
        switch (opt) {
            case 'g':
                parsed_args->grep_command = malloc(strlen(optarg) + 1);
                strcpy(parsed_args->grep_command, optarg);
                break;
        }
    }

    // After processing options, get the first non-option argument as pattern
    if (optind < argc) {
        parsed_args->pattern = malloc(strlen(argv[optind]) + 1);
        strcpy(parsed_args->pattern, argv[optind]);
    }

    return parsed_args;
}


void deallocate_arguments(arguments_t ** args) {
    if (args && *args) {
        if ((*args)->grep_command) {
            free((*args)->grep_command);
        }
        if ((*args)->pattern) {
            free((*args)->pattern);
        }
        free(*args);
        *args = NULL;
    }
}
