#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "line_list.h"

#define INIT_LINES_SIZE 500

void deallocate_lines_contents(int length, line_list_item* items);

/* 
 * Reset all of the pane properties based on the pane's current position.
 * should be called whenever the pane is moved, or when reallocating the
 * backing list.
 */
void refresh_pane(line_list_t *l);

line_list_t* line_list_init() {
    line_list_t *l;
    
    l = malloc(sizeof(line_list_t));

    if (l == NULL) {
        // TOOD make this log to error 
        fprintf(stderr, "ERROR: line_list_init: failed to allocate\n");
        return NULL;
    }
    
    l->items = malloc(sizeof(line_list_item) * INIT_LINES_SIZE);
    l->length = 0;
    l->capacity = INIT_LINES_SIZE;
    l->pane = malloc(sizeof(line_list_pane_t));
    l->pane->position = 0;
    refresh_pane(l);

    return l;
}

void line_list_add(line_list_t *l, int s, char line[]) {
    line_list_item *new_items;
    line_list_item *items_to_free;
    char *line_copy;
    int i;

    // Expand items if needed
    if (l->length == l->capacity) {
        new_items = malloc(sizeof(line_list_item) * l->capacity*2);
        l->capacity *= 2;

        for (i = 0; i < l->length; i++) {
            new_items[i] = l->items[i];
        }
        
        items_to_free = l->items;
        l->items = new_items;

        free(items_to_free);
    }
    
    line_copy = malloc(sizeof(char) * (s + 1));
    strncpy(line_copy, line, s);
    line_copy[s] = '\0';
    l->items[l->length].contents = line_copy;
    l->items[l->length].selected = false;
    l->length++;
    refresh_pane(l);
}

void line_list_clear(line_list_t *l) {
    deallocate_lines_contents(l->length, l->items);
    l->length = 0;
    l->pane->position = 0;
    refresh_pane(l);
}

void line_list_scroll_pane(line_list_t *l, int n) {
    int new_position = l->pane->position + n;
    
    // Prevent scrolling beyond the end of the list
    if(new_position >= l->length){
        return; 
    }
    
    // Prevent scrolling before the beginning of the list
    if(new_position < 0){
        return; 
    }

    l->pane->position = new_position; 
    refresh_pane(l);
}

void line_list_deallocate(line_list_t **l) {
    free((*l)->pane);
    deallocate_lines_contents((*l)->length, (*l)->items);
    free((*l)->items);
    free((*l));
    *l = NULL; 
}

//---- Private Functions ----//
void refresh_pane(line_list_t *l) {
    l->pane->length = l->length - l->pane->position;
    l->pane->items = l->items + l->pane->position;
}


/* 
 * Deallocate the contents of line_list_item array.
 */
void deallocate_lines_contents(int length, line_list_item* items){
    int i;

    for (i = 0; i < length; i++) {
        free(items[i].contents);
    }
}
