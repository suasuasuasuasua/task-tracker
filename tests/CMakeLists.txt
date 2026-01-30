enable_testing()

add_executable(test_task test_tasks.cpp)
target_link_libraries(test_task PRIVATE todo::todo)
add_test(NAME test_task COMMAND test_task)

add_executable(test_todo test_todo.cpp)
target_link_libraries(test_todo PRIVATE todo::todo)

add_test(NAME test_todo COMMAND test_todo)
