#ifndef HANDLERS_H
#define HANDLERS_H

#include <curl/curl.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ResponseData {
    char* data;
    size_t size;
};

// Handler for GET /todos
void handle_list_todos(CURL* curl, struct ResponseData* response);

// Handler for GET /todos/:id
void handle_get_todo(CURL* curl, int id, struct ResponseData* response);

// Handler for POST /todos
void handle_create_todo(CURL* curl, const char* post_data, size_t post_size, struct ResponseData* response);

// Handler for PUT /todos/:id
void handle_update_todo(CURL* curl, int id, const char* post_data, size_t post_size, struct ResponseData* response);

// Handler for DELETE /todos/:id
void handle_delete_todo(CURL* curl, int id, struct ResponseData* response);

#ifdef __cplusplus
}
#endif

#endif