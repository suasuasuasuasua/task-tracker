enable_testing()
set(BUILD_GMOCK OFF)

add_executable(test_task test_tasks.cpp)
target_link_libraries(test_task PRIVATE todo::todo GTest::gtest_main)

add_executable(test_todo test_todo.cpp)
target_link_libraries(test_todo PRIVATE todo::todo GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_task)
gtest_discover_tests(test_todo)
