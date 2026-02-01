#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>

#include "task.h"
#include "todo_viewmodel.h"

//////////////////////////////////////////////////////////////////////////////
// Model logic
void TodoViewModel::add_task() {
  auto task_text = trim_string(addtask_input_text);
  if (task_text.empty()) {
    // TODO: add status bar logic
    std::cerr << "Cannot add an empty task\n";
    return;
  }

  auto uid = model.add_task(task_text);
  addtask_input_text.clear();

  refresh_tasks();

  // update the current selected task to the last inserted task
  auto it =
      std::find_if(filtered_tasks.cbegin(), filtered_tasks.cend(),
                   [uid](const Task &task) { return task.getUid() == uid; });
  if (it != filtered_tasks.end()) {
    selected_task_idx =
        static_cast<std::int32_t>(std::distance(filtered_tasks.cbegin(), it));
  }
}

void TodoViewModel::update_task() {
  if (filtered_tasks.empty() or not is_selected_task_valid()) {
    return;
  }

  auto task_text = trim_string(edittask_input_text);
  if (task_text.empty()) {
    // TODO: add status bar logic
    std::cerr << "Cannot edit an empty task\n";
    return;
  }

  auto uid = filtered_tasks.at(selected_task_idx).getUid();
  model.update_task(uid, task_text);

  refresh_tasks();
}

void TodoViewModel::delete_selected_task() {
  if (filtered_tasks.empty() or not is_selected_task_valid()) {
    return;
  }

  // get the task uid
  auto uid = filtered_tasks.at(selected_task_idx).getUid();
  model.delete_task(uid);
  refresh_tasks();

  // adjust the selected_task_idx to be within the bounds (0, size)
  selected_task_idx =
      std::clamp(selected_task_idx, 0,
                 static_cast<std::int32_t>(filtered_tasks.size() - 1));
}

void TodoViewModel::mark_task(Task::Status status) {
  if (filtered_tasks.empty() or not is_selected_task_valid()) {
    return;
  }

  auto uid = filtered_tasks.at(selected_task_idx).getUid();
  model.mark_task(uid, status);
  refresh_tasks();
}

//////////////////////////////////////////////////////////////////////////////
// ViewModel logic

// non-mutable refs getters
const std::string &TodoViewModel::get_addtask_input_text_const() const {
  return addtask_input_text;
}

const std::string &TodoViewModel::get_edittask_input_text_const() const {
  return edittask_input_text;
}

const std::vector<std::string> &TodoViewModel::get_task_entries_const() const {
  return tasks_entries;
}

const std::int32_t &TodoViewModel::get_selected_task_const() const {
  return selected_task_idx;
}
const bool &TodoViewModel::get_addtask_input_shown() const {
  return addtask_input_shown;
}
const bool &TodoViewModel::get_edittask_input_shown() const {
  return edittask_input_shown;
}

// mutable refs getters
std::vector<Task> TodoViewModel::get_tasks() const { return filtered_tasks; }

std::string &TodoViewModel::get_addtask_input_text() {
  return addtask_input_text;
}

std::string &TodoViewModel::get_edittask_input_text() {
  return edittask_input_text;
}

std::vector<std::string> &TodoViewModel::get_task_entries() {
  return tasks_entries;
}

std::int32_t &TodoViewModel::get_selected_task() { return selected_task_idx; }

//////////////////////////////////////////////////////////////////////////////
// View logic
// callbacks

std::function<void()> TodoViewModel::addtask_show() {
  return [this] { addtask_input_shown = true; };
}
std::function<void()> TodoViewModel::addtask_hide() {
  return [this] { addtask_input_shown = false; };
}
std::function<void()> TodoViewModel::edittask_show() {
  return [this] { edittask_input_shown = true; };
}
std::function<void()> TodoViewModel::edittask_hide() {
  return [this] { edittask_input_shown = false; };
}

//////////////////////////////////////////////////////////////////////////////
// Private helpers
void TodoViewModel::refresh_tasks() {
  filtered_tasks.clear();
  tasks_entries.clear();

  // TODO: add filtering logic

  std::stringstream ss;
  auto tasks = model.getTasks();
  for (const auto &t : tasks | std::views::values) {
    ss << t;
    filtered_tasks.push_back(t);
    // TODO: format the strings nicer?
    tasks_entries.emplace_back(ss.str());

    ss.str(std::string()); // clear the stringstream
  }
}

std::string TodoViewModel::trim_string(const std::string &str) {
  const auto first = str.find_first_not_of(" \t\n\r");
  if (first == std::string::npos) {
    // String is empty or all whitespace
    return std::string();
  }
  const auto last = str.find_last_not_of(" \t\n\r");

  return str.substr(first, last - first + 1);
}

bool TodoViewModel::is_selected_task_valid() {
  return 0 <= selected_task_idx && selected_task_idx < filtered_tasks.size();
}
