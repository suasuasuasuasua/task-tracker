add_executable(task-cli cli.cpp)
target_link_libraries(
  task-cli
  PRIVATE todo::todo
  PRIVATE argparse)

add_executable(task-tui tui.cpp)
target_link_libraries(
  task-tui
  PRIVATE todo::todo
  PRIVATE spdlog::spdlog
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
