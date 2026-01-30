#include "task_viewmodel.h"

#include <algorithm>
#include <format>
#include <ranges>

TaskTrackerViewModel::TaskTrackerViewModel(
    const std::filesystem::path &filepath)
    : filepath_(filepath) {
  model_.from_json(filepath_);
  refreshFilteredTasks();
}

// ==========================================================================
// Observable State
// ==========================================================================

const std::vector<Task> &TaskTrackerViewModel::getFilteredTasks() const {
  return filtered_tasks_;
}

int TaskTrackerViewModel::getSelectedIndex() const { return selected_index_; }

std::optional<Task::Status> TaskTrackerViewModel::getCurrentFilter() const {
  return getFilterFromIndex();
}

const std::string &TaskTrackerViewModel::getInputText() const {
  return input_text_;
}

const std::string &TaskTrackerViewModel::getStatusMessage() const {
  return status_message_;
}

const std::vector<std::string> &TaskTrackerViewModel::getTaskEntries() const {
  return task_entries_;
}

// ==========================================================================
// Mutable State
// ==========================================================================

std::string &TaskTrackerViewModel::inputText() { return input_text_; }

int &TaskTrackerViewModel::selectedIndex() { return selected_index_; }

int &TaskTrackerViewModel::filterIndex() { return filter_index_; }

// ==========================================================================
// Commands
// ==========================================================================

void TaskTrackerViewModel::addTask() {
  // Trim whitespace
  auto trimmed = input_text_;
  trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
  trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);

  if (trimmed.empty()) {
    status_message_ = "Cannot add empty task";
    return;
  }

  auto uid = model_.add_task(trimmed);
  input_text_.clear();
  refreshFilteredTasks();

  status_message_ = std::format("Task added (ID: {})", uid);
}

void TaskTrackerViewModel::deleteSelectedTask() {
  if (filtered_tasks_.empty()) {
    status_message_ = "No tasks to delete";
    return;
  }

  if (selected_index_ < 0 ||
      selected_index_ >= static_cast<int>(filtered_tasks_.size())) {
    status_message_ = "Invalid selection";
    return;
  }

  auto uid = filtered_tasks_[selected_index_].getUid();
  model_.delete_task(uid);
  refreshFilteredTasks();

  // Adjust selection if we deleted the last item
  if (selected_index_ >= static_cast<int>(filtered_tasks_.size())) {
    selected_index_ =
        std::max(0, static_cast<int>(filtered_tasks_.size()) - 1);
  }

  status_message_ = std::format("Task deleted (ID: {})", uid);
}

void TaskTrackerViewModel::markSelectedTask(Task::Status status) {
  if (filtered_tasks_.empty()) {
    status_message_ = "No tasks to mark";
    return;
  }

  if (selected_index_ < 0 ||
      selected_index_ >= static_cast<int>(filtered_tasks_.size())) {
    status_message_ = "Invalid selection";
    return;
  }

  auto uid = filtered_tasks_[selected_index_].getUid();
  model_.mark_task(uid, status);
  refreshFilteredTasks();

  status_message_ =
      std::format("Task marked as {} (ID: {})", Task::Stat2String.at(status), uid);
}

void TaskTrackerViewModel::applyFilter() {
  refreshFilteredTasks();
  selected_index_ = 0; // Reset selection when filter changes
}

void TaskTrackerViewModel::save() { model_.to_json(filepath_); }

// ==========================================================================
// Presentation Helpers
// ==========================================================================

std::string TaskTrackerViewModel::formatTaskForDisplay(const Task &task) {
  return std::format("{}. {} [{}]", task.getUid(), task.getDesc(),
                     Task::Stat2String.at(task.getStatus()));
}

ftxui::Color TaskTrackerViewModel::getStatusColor(Task::Status status) {
  using namespace ftxui;
  switch (status) {
  case Task::Status::ToDo:
    return Color::Yellow;
  case Task::Status::InProgress:
    return Color::Blue;
  case Task::Status::Done:
    return Color::Green;
  }
  return Color::White;
}

const std::vector<std::string> &TaskTrackerViewModel::getFilterOptions() {
  return filter_options_;
}

// ==========================================================================
// Private Helpers
// ==========================================================================

void TaskTrackerViewModel::refreshFilteredTasks() {
  filtered_tasks_.clear();
  task_entries_.clear();

  auto filter = getFilterFromIndex();
  const auto &tasks = model_.getTasks();

  for (const auto &task : tasks) {
    // If no filter, include all tasks; otherwise check status
    if (!filter.has_value() || task.getStatus() == filter.value()) {
      filtered_tasks_.push_back(task);
      task_entries_.push_back(formatTaskForDisplay(task));
    }
  }
}

std::optional<Task::Status> TaskTrackerViewModel::getFilterFromIndex() const {
  switch (filter_index_) {
  case 1:
    return Task::Status::ToDo;
  case 2:
    return Task::Status::InProgress;
  case 3:
    return Task::Status::Done;
  default:
    return std::nullopt; // Show all
  }
}
