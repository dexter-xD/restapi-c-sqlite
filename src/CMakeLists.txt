add_subdirectory(core)
add_subdirectory(http)
add_subdirectory(db)

add_executable(todo_api main.c)

target_link_libraries(todo_api
    PRIVATE
    todo_core
    todo_http
    todo_db
    SQLite::SQLite3
    CURL::libcurl
)

install(TARGETS todo_api
    RUNTIME DESTINATION bin
) 