add_library(todo)
target_sources(
  todo
  PRIVATE task.cpp todo.cpp todo_viewmodel.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_SOURCE_DIR}/inc)
add_library(todo::todo ALIAS todo)
target_link_libraries(todo PUBLIC nlohmann_json::nlohmann_json)

add_executable(task-cli ${CMAKE_SOURCE_DIR}/app/cli.cpp)
target_link_libraries(task-cli PRIVATE todo::todo)

add_executable(task-tui ${CMAKE_SOURCE_DIR}/app/tui.cpp)
target_link_libraries(
  task-tui
  PUBLIC todo::todo
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component)

set_target_properties(
  task-cli task-tui
  PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
             RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

install(TARGETS task-cli DESTINATION bin)
install(TARGETS task-tui DESTINATION bin)
