#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "line_list.h"

#define INIT_LINES_SIZE 500

void deallocate_lines_contents(int length, char** lines);

line_list_t* line_list_init() {
    line_list_t *l;
    
    l = malloc(sizeof(line_list_t));

    if (l == NULL) {
        // TOOD make this log to error 
        printf("ERROR: line_list_init: failed to allocate");
        exit(1);
    }
    
    l->lines = malloc(sizeof(char*) * INIT_LINES_SIZE);
    l->length = 0;
    l->capacity = INIT_LINES_SIZE;

    return l;
}

void line_list_add(line_list_t *l, int s, char line[]) {
    char **new_lines;
    char **lines_to_free;
    char *line_copy;
    int i;

    // Expand lines if needed
    if (l->length == l->capacity) {
        new_lines = malloc(sizeof(char*) * l->capacity*2);
        l->capacity *= 2;

        for (i = 0; i < l->length; i++) {
            new_lines[i] = l->lines[i];
        }
        
        lines_to_free = l->lines;
        l->lines = new_lines;

        free(lines_to_free);
    }
    
    line_copy = malloc(sizeof(char) * (s + 1));
    strncpy(line_copy, line, s);
    line_copy[s] = '\0';
    l->lines[l->length] = line_copy;
    l->length++;
}

void line_list_clear(line_list_t *l) {
    deallocate_lines_contents(l->length, l->lines);
    l->length = 0;
}

void line_list_deallocate(line_list_t **l) {
    deallocate_lines_contents((*l)->length, (*l)->lines);
    free((*l)->lines);
    free((*l));
    *l = NULL; 
}

/* 
 * Deallocate the contents of char* list lines.
 */
void deallocate_lines_contents(int length, char** lines){
    int i;

    for (i = 0; i < length; i++) {
        free(lines[i]);
    }
}
