#ifndef INCLUDE_INC_UI_STATE_H_
#define INCLUDE_INC_UI_STATE_H_

#include <cstdint>
#include <string>
#include <vector>

#include "task.h"

/// @brief Manages UI-specific state separate from business logic
class UIState {
public:
  UIState() = default;

  // Modal visibility state
  [[nodiscard]] bool is_add_task_modal_shown() const {
    return add_task_modal_shown_;
  }
  [[nodiscard]] bool is_edit_task_modal_shown() const {
    return edit_task_modal_shown_;
  }
  
  // Non-const accessors for FTXUI Modal binding
  bool &add_task_modal_shown() { return add_task_modal_shown_; }
  bool &edit_task_modal_shown() { return edit_task_modal_shown_; }

  void show_add_task_modal() { add_task_modal_shown_ = true; }
  void hide_add_task_modal() {
    add_task_modal_shown_ = false;
    add_task_input_.clear();
  }

  void show_edit_task_modal() { edit_task_modal_shown_ = true; }
  void hide_edit_task_modal() {
    edit_task_modal_shown_ = false;
    edit_task_input_.clear();
  }

  // Input text management
  [[nodiscard]] const std::string &add_task_input() const {
    return add_task_input_;
  }
  std::string &add_task_input() { return add_task_input_; }

  [[nodiscard]] const std::string &edit_task_input() const {
    return edit_task_input_;
  }
  std::string &edit_task_input() { return edit_task_input_; }

  // Task selection
  [[nodiscard]] std::int32_t selected_index() const {
    return selected_task_idx_;
  }
  std::int32_t &selected_index() { return selected_task_idx_; }

  [[nodiscard]] bool is_selection_valid(std::size_t task_count) const {
    return selected_task_idx_ >= 0 &&
           static_cast<std::size_t>(selected_task_idx_) < task_count;
  }

  void clamp_selection(std::size_t task_count) {
    if (task_count == 0) {
      selected_task_idx_ = -1; // No valid selection when empty
    } else {
      selected_task_idx_ = std::clamp(
          selected_task_idx_, 0, static_cast<std::int32_t>(task_count - 1));
    }
  }

  // Display entries
  [[nodiscard]] const std::vector<std::string> &task_entries() const {
    return task_entries_;
  }
  std::vector<std::string> &task_entries() { return task_entries_; }

private:
  bool add_task_modal_shown_{false};
  bool edit_task_modal_shown_{false};
  std::string add_task_input_;
  std::string edit_task_input_;
  std::int32_t selected_task_idx_{0};
  std::vector<std::string> task_entries_;
};

#endif // INCLUDE_INC_UI_STATE_H_
