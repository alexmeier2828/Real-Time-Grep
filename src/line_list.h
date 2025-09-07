#ifndef LINE_LIST_H
#define LINE_LIST_H

typedef struct {
    int length;             // The number of lines within the pane.
    int position;           // The position of the pane within the larger backing list.
    char **lines;           // Pointer to the first line within the pane. 
} line_list_pane_t;

typedef struct {
    int length;             // Number of lines in the list.
    int capacity;           // Current allocated space for the list.
    line_list_pane_t *pane;  // A subset of the lines to display in the pane.
    char **lines;           // The full list of lines.
} line_list_t;

line_list_t* line_list_init();
void line_list_add(line_list_t *l, int s, char line[]);
void line_list_clear(line_list_t *l);
void line_list_scroll_pane(line_list_t *l, int n);
void line_list_deallocate(line_list_t **l);

#endif 


