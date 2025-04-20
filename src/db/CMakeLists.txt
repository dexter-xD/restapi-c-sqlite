add_library(todo_db
    database.c
)

target_include_directories(todo_db
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(todo_db
    PRIVATE
    SQLite::SQLite3
    todo_core
) 