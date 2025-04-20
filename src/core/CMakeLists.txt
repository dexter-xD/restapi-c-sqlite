add_library(todo_core
    todo.c
)

target_include_directories(todo_core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(todo_core
    PRIVATE
    SQLite::SQLite3
) 