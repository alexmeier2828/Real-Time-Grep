#ifndef ARGUMENTS_H
#define ARGUMENTS_H

typedef struct {
    char *grep_command;
    char *pattern;
} arguments_t;

arguments_t* get_cli_arguments(int argc, char **argv);
void print_usage(const char *program_name);
void deallocate_arguments(arguments_t ** args);

#endif

