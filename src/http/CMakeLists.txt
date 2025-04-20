add_library(todo_http
    server.c
    handlers.c
)

target_include_directories(todo_http
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
    ${MICROHTTPD_INCLUDE_DIR}
    ${JANSSON_INCLUDE_DIRS}
)

target_link_libraries(todo_http
    PRIVATE
    todo_core
    todo_db
    CURL::libcurl
    ${MICROHTTPD_LIBRARY}
    ${JANSSON_LIBRARIES}
) 