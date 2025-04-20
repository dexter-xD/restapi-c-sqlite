#include "todo.h"
#include "../db/database.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

int todo_create(const char* title, const char* description) {
    if (!title || !description) {
        return -1;
    }

    time_t now = time(NULL);
    return db_execute_query(
        "INSERT INTO todos (title, description, completed, created_at, updated_at) "
        "VALUES (?, ?, 0, ?, ?)",
        "text", title,
        "text", description,
        "int", (long long)now,
        "int", (long long)now
    );
}

int todo_get(int id, todo_t* todo) {
    if (!todo) {
        return -1;
    }

    return db_get_todo(id, todo);
}

int todo_update(int id, const char* title, const char* description, int completed) {
    if (!title || !description) {
        return -1;
    }

    time_t now = time(NULL);
    return db_execute_query(
        "UPDATE todos SET title = ?, description = ?, completed = ?, updated_at = ? "
        "WHERE id = ?",
        "text", title,
        "text", description,
        "int", (long long)completed,
        "int", (long long)now,
        "int", (long long)id
    );
}

int todo_delete(int id) {
    return db_execute_query(
        "DELETE FROM todos WHERE id = ?",
        "int", (long long)id
    );
}

int todo_list(todo_t** todos, int* count) {
    if (!todos || !count) {
        return -1;
    }

    return db_get_todos(todos, count);
}

void todo_free_list(todo_t* todos) {
    free(todos);
} 