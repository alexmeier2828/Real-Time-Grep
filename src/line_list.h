#ifndef LINE_LIST_H
#define LINE_LIST_H

#include <stdbool.h>

typedef struct { 
    char *contents;              // null terminated string with one line of grep results.
    bool selected;           // true if this line is selected in the UI.
} line_list_item;

typedef struct {
    int length;             // The number of lines within the pane.
    int position;           // The position of the pane within the larger backing list.
    line_list_item *items;           // Pointer to the first line within the pane. 
} line_list_pane_t;

typedef struct {
    int length;             // Number of lines in the list.
    int capacity;           // Current allocated space for the list.
    line_list_pane_t *pane;  // A subset of the lines to display in the pane.
    line_list_item *items;           // The full list of line items.
} line_list_t;

line_list_t* line_list_init();
void line_list_add(line_list_t *l, int s, char line[]);
void line_list_clear(line_list_t *l);
void line_list_scroll_pane(line_list_t *l, int n);
void line_list_deallocate(line_list_t **l);

#endif 


