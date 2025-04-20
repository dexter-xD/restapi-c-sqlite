#include "handlers.h"
#include "../core/todo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include <curl/curl.h>

struct MemoryStruct {
    char* memory;
    size_t size;
};

static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    char* ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

void handle_list_todos(CURL* curl, struct ResponseData* response) {
    (void)curl; 
    todo_t* todos = NULL;
    int count = 0;

    if (todo_list(&todos, &count) == 0) {
        json_t* root = json_array();
        
        for (int i = 0; i < count; i++) {
            json_t* todo = json_object();
            json_object_set_new(todo, "id", json_integer(todos[i].id));
            json_object_set_new(todo, "title", json_string(todos[i].title));
            json_object_set_new(todo, "description", json_string(todos[i].description));
            json_object_set_new(todo, "completed", json_boolean(todos[i].completed));
            json_object_set_new(todo, "created_at", json_integer(todos[i].created_at));
            json_object_set_new(todo, "updated_at", json_integer(todos[i].updated_at));
            json_array_append_new(root, todo);
        }

        response->data = json_dumps(root, JSON_INDENT(2));
        response->size = strlen(response->data);

        json_decref(root);
        todo_free_list(todos);
    } else {
        response->data = strdup("{\"error\": \"Failed to list todos\"}");
        response->size = strlen(response->data);
    }
}

void handle_get_todo(CURL* curl, int id, struct ResponseData* response) {
    (void)curl; 
    todo_t todo;
    if (todo_get(id, &todo) == 0) {
        json_t* root = json_object();
        json_object_set_new(root, "id", json_integer(todo.id));
        json_object_set_new(root, "title", json_string(todo.title));
        json_object_set_new(root, "description", json_string(todo.description));
        json_object_set_new(root, "completed", json_boolean(todo.completed));
        json_object_set_new(root, "created_at", json_integer(todo.created_at));
        json_object_set_new(root, "updated_at", json_integer(todo.updated_at));

        response->data = json_dumps(root, JSON_INDENT(2));
        response->size = strlen(response->data);

        json_decref(root);
    } else {
        response->data = strdup("{\"error\": \"Todo not found\"}");
        response->size = strlen(response->data);
    }
}

void handle_create_todo(CURL* curl, const char* post_data, size_t post_size, struct ResponseData* response) {
    (void)curl;
    
    printf("DEBUG: handle_create_todo received %zu bytes of post data\n", post_size);
    if (post_data) {
        printf("DEBUG: post_data = '%.*s'\n", (int)post_size, post_data);
    } else {
        printf("DEBUG: post_data is NULL\n");
    }

    if (post_data && post_size > 0) {
        json_error_t error;
        json_t* root = json_loadb(post_data, post_size, 0, &error);
        
        if (root && json_is_object(root)) {
            printf("DEBUG: Successfully parsed JSON\n");
            const char* title = json_string_value(json_object_get(root, "title"));
            const char* description = json_string_value(json_object_get(root, "description"));

            printf("DEBUG: title = %s, description = %s\n", 
                   title ? title : "NULL", 
                   description ? description : "NULL");

            if (title && description) {
                if (todo_create(title, description) == 0) {
                    printf("DEBUG: Todo created successfully\n");
                    response->data = strdup("{\"status\": \"Todo created successfully\"}");
                } else {
                    printf("DEBUG: Failed to create todo\n");
                    response->data = strdup("{\"error\": \"Failed to create todo\"}");
                }
            } else {
                printf("DEBUG: Invalid request data\n");
                response->data = strdup("{\"error\": \"Invalid request data\"}");
            }

            json_decref(root);
        } else {
            printf("DEBUG: Invalid JSON data. Error: %s\n", error.text);
            response->data = strdup("{\"error\": \"Invalid JSON data\"}");
        }
    } else {
        printf("DEBUG: No data received\n");
        response->data = strdup("{\"error\": \"No data received\"}");
    }

    response->size = strlen(response->data);
}

void handle_update_todo(CURL* curl, int id, const char* post_data, size_t post_size, struct ResponseData* response) {
    (void)curl;
    
    printf("DEBUG: handle_update_todo received %zu bytes of post data\n", post_size);
    if (post_data) {
        printf("DEBUG: post_data = '%.*s'\n", (int)post_size, post_data);
    } else {
        printf("DEBUG: post_data is NULL\n");
    }

    if (post_data && post_size > 0) {
        json_error_t error;
        json_t* root = json_loadb(post_data, post_size, 0, &error);
        
        if (root && json_is_object(root)) {
            printf("DEBUG: Successfully parsed JSON\n");
            const char* title = json_string_value(json_object_get(root, "title"));
            const char* description = json_string_value(json_object_get(root, "description"));
            json_t* completed_json = json_object_get(root, "completed");
            int completed = completed_json ? json_boolean_value(completed_json) : 0;

            printf("DEBUG: title = %s, description = %s, completed = %d\n", 
                   title ? title : "NULL", 
                   description ? description : "NULL",
                   completed);

            if (title && description) {
                if (todo_update(id, title, description, completed) == 0) {
                    printf("DEBUG: Todo updated successfully\n");
                    response->data = strdup("{\"status\": \"Todo updated successfully\"}");
                } else {
                    printf("DEBUG: Failed to update todo\n");
                    response->data = strdup("{\"error\": \"Failed to update todo\"}");
                }
            } else {
                printf("DEBUG: Invalid request data\n");
                response->data = strdup("{\"error\": \"Invalid request data\"}");
            }

            json_decref(root);
        } else {
            printf("DEBUG: Invalid JSON data. Error: %s\n", error.text);
            response->data = strdup("{\"error\": \"Invalid JSON data\"}");
        }
    } else {
        printf("DEBUG: No data received\n");
        response->data = strdup("{\"error\": \"No data received\"}");
    }

    response->size = strlen(response->data);
}

void handle_delete_todo(CURL* curl, int id, struct ResponseData* response) {
    (void)curl;
    
    if (todo_delete(id) == 0) {
        response->data = strdup("{\"status\": \"Todo deleted successfully\"}");
    } else {
        response->data = strdup("{\"error\": \"Failed to delete todo\"}");
    }
    response->size = strlen(response->data);
} 