#ifndef INCLUDE_INC_TODO_VIEWMODEL_H_
#define INCLUDE_INC_TODO_VIEWMODEL_H_

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "todo.h"
#include "ui_state.h"

/// @brief View model that mediates between the TodoTracker model and UI views
/// Follows MVVM pattern with clear separation of concerns
class TodoViewModel {
public:
  explicit TodoViewModel(std::filesystem::path filepath);
  ~TodoViewModel();

  // Prevent copying, allow moving
  TodoViewModel(const TodoViewModel &) = delete;
  TodoViewModel &operator=(const TodoViewModel &) = delete;
  TodoViewModel(TodoViewModel &&) = default;
  TodoViewModel &operator=(TodoViewModel &&) = default;

  //////////////////////////////////////////////////////////////////////////////
  // Task Management - Business Logic Operations
  void add_task();
  void update_task();
  void delete_selected_task();
  void mark_selected_task(Task::Status status);

  //////////////////////////////////////////////////////////////////////////////
  // Query Methods - Const access to state
  [[nodiscard]] std::optional<Task> get_selected_task() const;
  [[nodiscard]] const std::vector<Task> &filtered_tasks() const {
    return filtered_tasks_;
  }

  //////////////////////////////////////////////////////////////////////////////
  // UI State Access - Direct access to UI state manager
  [[nodiscard]] UIState &ui_state() { return ui_state_; }
  [[nodiscard]] const UIState &ui_state() const { return ui_state_; }

private:
  // Core data
  TodoTracker model_;
  std::filesystem::path filepath_;

  // View state
  UIState ui_state_;
  std::vector<Task> filtered_tasks_;

  // Helper methods
  void refresh_tasks();
  [[nodiscard]] static std::string trim(std::string_view str);
};

#endif // INCLUDE_INC_TODO_VIEWMODEL_H_
