#ifndef TODO_H
#define TODO_H

#include <time.h>

typedef struct {
    int id;
    char title[256];
    char description[1024];
    int completed;
    time_t created_at;
    time_t updated_at;
} todo_t;

int todo_create(const char* title, const char* description);
int todo_get(int id, todo_t* todo);
int todo_update(int id, const char* title, const char* description, int completed);
int todo_delete(int id);
int todo_list(todo_t** todos, int* count);
void todo_free_list(todo_t* todos);

#endif