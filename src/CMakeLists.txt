add_library(todo)
target_sources(
  todo
  PRIVATE task.cpp todo.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_SOURCE_DIR}/inc)
add_library(todo::todo ALIAS todo)
target_link_libraries(todo PUBLIC nlohmann_json::nlohmann_json)

add_executable(task-cli main.cpp)
target_link_libraries(task-cli PRIVATE todo::todo)

install(TARGETS task-cli DESTINATION bin)
