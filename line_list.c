#include <stdlib.h>
#include <stdio.h>
#include "line_list.h"

line_list_t* line_list_init() {
    line_list_t *l;
    
    l = malloc(sizeof(line_list_t));

    if (l == NULL) {
        // TOOD make this log to error 
        printf("ERROR: line_list_init: failed to allocate");
        exit(1);
    }
    

    // TODO allocate line list
    return l;
}

void line_list_add(line_list_t *l, char line[]) {
    char **new_lines;
    char **lines_to_free;
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
    
    // TODO the line that gets passed in here should actually be a buffer that
    // gets reused by the caller, so here we need to strcopy to a heap
    // allocated string. 
    l->lines[l->length] = line;
    l->length++;
}

void lise_list_deallocate(line_list_t **l) {
    free((*l)->lines);
    free((*l));
    *l = NULL; 
}
