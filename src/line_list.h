#ifndef LINE_LIST_H
#define LINE_LIST_H

typedef struct {
    int length;
    int capacity;
    char **lines;
} line_list_t;

line_list_t* line_list_init();
void line_list_add(line_list_t *l, int s, char line[]);
void line_list_clear(line_list_t *l);
void line_list_deallocate(line_list_t **l);

#endif 
