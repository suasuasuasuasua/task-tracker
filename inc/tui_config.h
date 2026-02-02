#ifndef INCLUDE_INC_TUI_CONFIG_H_
#define INCLUDE_INC_TUI_CONFIG_H_

#include <string_view>

namespace tui_config {

// UI Constants
constexpr int modal_min_width = 80;

// Window titles
constexpr std::string_view app_title = "Task Tracker";
constexpr std::string_view tasks_window_title = "Tasks";
constexpr std::string_view add_task_prompt = "Enter the task";
constexpr std::string_view edit_task_prompt = "Edit the task";

// Logging
constexpr std::string_view logger_name = "logger";

// Keyboard shortcuts (for documentation)
namespace shortcuts {
  constexpr std::string_view add_task = "n";
  constexpr std::string_view edit_task = "e";
  constexpr std::string_view delete_task = "d";
  constexpr std::string_view mark_todo = "t";
  constexpr std::string_view mark_in_progress = "i";
  constexpr std::string_view mark_done = "m";
  constexpr std::string_view quit = "q";
  constexpr std::string_view escape = "ESC";
} // namespace shortcuts

} // namespace tui_config

#endif // INCLUDE_INC_TUI_CONFIG_H_
