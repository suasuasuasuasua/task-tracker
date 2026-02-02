#include <algorithm>
#include <iostream>
#include <sstream>

#include "task.h"
#include "todo_viewmodel.h"

TodoViewModel::TodoViewModel(std::filesystem::path filepath)
    : filepath_(std::move(filepath)) {
  model_.from_json(filepath_);
  refresh_tasks();
}

TodoViewModel::~TodoViewModel() { model_.to_json(filepath_); }

//////////////////////////////////////////////////////////////////////////////
// Task Management - Business Logic Operations

void TodoViewModel::add_task() {
  auto task_text = trim(ui_state_.add_task_input());
  if (task_text.empty()) {
    // TODO: add status bar logic
    std::cerr << "Cannot add an empty task\n";
    return;
  }

  auto uid = model_.add_task(task_text);
  ui_state_.hide_add_task_modal();

  refresh_tasks();

  // Update selection to newly added task
  auto it = std::find_if(filtered_tasks_.cbegin(), filtered_tasks_.cend(),
                         [uid](const Task &task) { return task.getUid() == uid; });
  if (it != filtered_tasks_.cend()) {
    ui_state_.selected_index() =
        static_cast<std::int32_t>(std::distance(filtered_tasks_.cbegin(), it));
  }
}

void TodoViewModel::update_task() {
  if (!ui_state_.is_selection_valid(filtered_tasks_.size())) {
    return;
  }

  auto task_text = trim(ui_state_.edit_task_input());
  if (task_text.empty()) {
    // TODO: add status bar logic
    std::cerr << "Cannot edit an empty task\n";
    return;
  }

  auto uid = filtered_tasks_[ui_state_.selected_index()].getUid();
  model_.update_task(uid, task_text);

  ui_state_.hide_edit_task_modal();
  refresh_tasks();
}

void TodoViewModel::delete_selected_task() {
  if (!ui_state_.is_selection_valid(filtered_tasks_.size())) {
    return;
  }

  auto uid = filtered_tasks_[ui_state_.selected_index()].getUid();
  model_.delete_task(uid);
  refresh_tasks();

  // Adjust selection to be within bounds
  ui_state_.clamp_selection(filtered_tasks_.size());
}

void TodoViewModel::mark_selected_task(Task::Status status) {
  if (!ui_state_.is_selection_valid(filtered_tasks_.size())) {
    return;
  }

  auto uid = filtered_tasks_[ui_state_.selected_index()].getUid();
  model_.mark_task(uid, status);
  refresh_tasks();
}

//////////////////////////////////////////////////////////////////////////////
// Query Methods

std::optional<Task> TodoViewModel::get_selected_task() const {
  if (!ui_state_.is_selection_valid(filtered_tasks_.size())) {
    return std::nullopt;
  }
  return filtered_tasks_[ui_state_.selected_index()];
}

//////////////////////////////////////////////////////////////////////////////
// Private Helper Methods

void TodoViewModel::refresh_tasks() {
  filtered_tasks_.clear();
  ui_state_.task_entries().clear();

  // TODO: add filtering logic

  std::stringstream ss;
  auto tasks = model_.getTasks();
  for (const auto &task : tasks | std::views::values) {
    ss << task;
    filtered_tasks_.push_back(task);
    ui_state_.task_entries().emplace_back(ss.str());
    ss.str(std::string()); // clear the stringstream
  }
}

std::string TodoViewModel::trim(std::string_view str) {
  const auto first = str.find_first_not_of(" \t\n\r");
  if (first == std::string_view::npos) {
    return std::string();
  }
  const auto last = str.find_last_not_of(" \t\n\r");
  return std::string(str.substr(first, last - first + 1));
}
