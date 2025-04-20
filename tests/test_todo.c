#include "../src/core/todo.h"
#include "../src/db/database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void test_create_todo(void) {
    assert(db_init(":memory:") == 0);
    
    assert(todo_create("Test Todo", "Test Description") == 0);
    
    todo_t todo;
    assert(todo_get(1, &todo) == 0);
    assert(strcmp(todo.title, "Test Todo") == 0);
    assert(strcmp(todo.description, "Test Description") == 0);
    assert(todo.completed == 0);
    
    db_cleanup();
}

void test_update_todo(void) {
    assert(db_init(":memory:") == 0);
    
    assert(todo_create("Test Todo", "Test Description") == 0);
    assert(todo_update(1, "Updated Todo", "Updated Description", 1) == 0);
    
    todo_t todo;
    assert(todo_get(1, &todo) == 0);
    assert(strcmp(todo.title, "Updated Todo") == 0);
    assert(strcmp(todo.description, "Updated Description") == 0);
    assert(todo.completed == 1);
    
    db_cleanup();
}

void test_delete_todo(void) {
    assert(db_init(":memory:") == 0);
    
    assert(todo_create("Test Todo", "Test Description") == 0);
    assert(todo_delete(1) == 0);
    
    todo_t todo;
    assert(todo_get(1, &todo) != 0);
    
    db_cleanup();
}

void test_list_todos(void) {
    assert(db_init(":memory:") == 0);
    
    assert(todo_create("Todo 1", "Description 1") == 0);
    assert(todo_create("Todo 2", "Description 2") == 0);
    
    todo_t* todos = NULL;
    int count = 0;
    assert(todo_list(&todos, &count) == 0);
    assert(count == 2);
    assert(strcmp(todos[0].title, "Todo 1") == 0);
    assert(strcmp(todos[1].title, "Todo 2") == 0);
    
    todo_free_list(todos);
    db_cleanup();
}

int main(void) {
    printf("Running tests...\n");
    
    test_create_todo();
    test_update_todo();
    test_delete_todo();
    test_list_todos();
    
    printf("All tests passed!\n");
    return EXIT_SUCCESS;
} 