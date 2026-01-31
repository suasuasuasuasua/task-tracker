add_library(todo)
target_sources(
  todo
  PRIVATE task.cpp todo.cpp todo_viewmodel.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_SOURCE_DIR}/inc)
add_library(todo::todo ALIAS todo)
target_link_libraries(todo PUBLIC nlohmann_json::nlohmann_json)
