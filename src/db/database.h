#ifndef DATABASE_H
#define DATABASE_H

#include "../core/todo.h"


int db_init(const char* db_path);
void db_cleanup(void);
int db_execute_query(const char* query, ...);
int db_get_todo(int id, todo_t* todo);
int db_get_todos(todo_t** todos, int* count);

#endif 