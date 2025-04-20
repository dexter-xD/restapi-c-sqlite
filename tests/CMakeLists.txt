enable_testing()

add_executable(test_todo
    test_todo.c
)

target_link_libraries(test_todo
    PRIVATE
    todo_core
    todo_db
)

add_test(NAME test_todo COMMAND test_todo) 