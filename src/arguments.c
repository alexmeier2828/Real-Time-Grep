#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include "arguments.h"


arguments_t* get_cli_arguments(int argc, char **argv) {
    int opt;
    arguments_t* parsed_args;

    // Reset getopt state for multiple calls
    optind = 1;
    
    parsed_args = malloc(sizeof(arguments_t));
    parsed_args->pattern = NULL;
    parsed_args->grep_command = NULL;

    while((opt = getopt(argc, argv, ":g:h")) != -1) {
        switch (opt) {
            case 'g':
                parsed_args->grep_command = malloc(strlen(optarg) + 1);
                strcpy(parsed_args->grep_command, optarg);
                break;
            case 'h':
                print_usage(argv[0]);
                deallocate_arguments(&parsed_args);
                exit(0);
            case ':':
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                print_usage(argv[0]);
                deallocate_arguments(&parsed_args);
                return NULL;
            case '?':
                fprintf(stderr, "Unknown option: -%c\n", optopt);
                print_usage(argv[0]);
                deallocate_arguments(&parsed_args);
                return NULL;
        }
    }

    // After processing options, get the first non-option argument as pattern
    if (optind < argc) {
        parsed_args->pattern = malloc(strlen(argv[optind]) + 1);
        strcpy(parsed_args->pattern, argv[optind]);
        
        // Check for extra arguments after pattern
        if (optind + 1 < argc) {
            fprintf(stderr, "Error: Too many arguments.\n");
            print_usage(argv[0]);
            deallocate_arguments(&parsed_args);
            return NULL;
        }
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

void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS] [PATTERN]\n", program_name);
    printf("\n");
    printf("Real-time grep utility\n");
    printf("\n");
    printf("Arguments:\n");
    printf("  PATTERN                 Search pattern to match (optional)\n");
    printf("\n");
    printf("Options:\n");
    printf("  -g COMMAND             Custom grep command to use\n");
    printf("  -h, --help             Show this help message\n");
}
