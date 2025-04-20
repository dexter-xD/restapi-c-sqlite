#include "database.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static sqlite3* db = NULL;

int db_init(const char* db_path) {
    if (sqlite3_open(db_path, &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    const char* create_table_sql = 
        "CREATE TABLE IF NOT EXISTS todos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "description TEXT,"
        "completed INTEGER DEFAULT 0,"
        "created_at INTEGER,"
        "updated_at INTEGER"
        ")";

    char* err_msg = NULL;
    if (sqlite3_exec(db, create_table_sql, NULL, NULL, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    return 0;
}

void db_cleanup(void) {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

int db_execute_query(const char* query, ...) {
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    va_list args;
    va_start(args, query);
    
    int param_count = sqlite3_bind_parameter_count(stmt);
    for (int i = 1; i <= param_count; i++) {
        const char* param_type = va_arg(args, const char*);
        if (strcmp(param_type, "text") == 0) {
            const char* text_val = va_arg(args, const char*);
            sqlite3_bind_text(stmt, i, text_val, -1, SQLITE_STATIC);
        } else if (strcmp(param_type, "int") == 0) {
            long long int_val = va_arg(args, long long);
            sqlite3_bind_int64(stmt, i, int_val);
        }
    }

    va_end(args);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}

int db_get_todo(int id, todo_t* todo) {
    const char* query = "SELECT * FROM todos WHERE id = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        return -1;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        todo->id = sqlite3_column_int(stmt, 0);
        const unsigned char* title = sqlite3_column_text(stmt, 1);
        const unsigned char* desc = sqlite3_column_text(stmt, 2);
        if (title) strncpy(todo->title, (const char*)title, sizeof(todo->title) - 1);
        if (desc) strncpy(todo->description, (const char*)desc, sizeof(todo->description) - 1);
        todo->completed = sqlite3_column_int(stmt, 3);
        todo->created_at = sqlite3_column_int64(stmt, 4);
        todo->updated_at = sqlite3_column_int64(stmt, 5);
        sqlite3_finalize(stmt);
        return 0;
    }

    sqlite3_finalize(stmt);
    return -1;
}

int db_get_todos(todo_t** todos, int* count) {
    const char* query = "SELECT COUNT(*) FROM todos";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        return -1;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return -1;
    }

    *count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (*count == 0) {
        *todos = NULL;
        return 0;
    }

    *todos = malloc(sizeof(todo_t) * *count);
    if (!*todos) {
        return -1;
    }

    query = "SELECT * FROM todos";
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        free(*todos);
        return -1;
    }

    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        todo_t* todo = &(*todos)[i++];
        todo->id = sqlite3_column_int(stmt, 0);
        const unsigned char* title = sqlite3_column_text(stmt, 1);
        const unsigned char* desc = sqlite3_column_text(stmt, 2);
        if (title) strncpy(todo->title, (const char*)title, sizeof(todo->title) - 1);
        if (desc) strncpy(todo->description, (const char*)desc, sizeof(todo->description) - 1);
        todo->completed = sqlite3_column_int(stmt, 3);
        todo->created_at = sqlite3_column_int64(stmt, 4);
        todo->updated_at = sqlite3_column_int64(stmt, 5);
    }

    sqlite3_finalize(stmt);
    return 0;
} 