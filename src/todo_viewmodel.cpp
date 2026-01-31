#include "todo_viewmodel.h"
#include <iostream>
#include <sstream>

//////////////////////////////////////////////////////////////////////////////
// Model logic
void TodoViewModel::add_task() {
  auto task_text = input_text;
  // task_text.erase(0, task_text.find_first_not_of(" \t\n\r"));
  // task_text.erase(task_text.find_last_not_of(" \t\n\r") + 1);

  if (task_text.empty()) {
    // TODO: add status bar logic
    std::cerr << "Cannot add an empty task\n";
    return;
  }

  auto uid = model.add_task(task_text);
  input_text.clear();

  refresh_tasks();
}

//////////////////////////////////////////////////////////////////////////////
// ViewModel logic

// non-mutable refs getters
const std::string &TodoViewModel::get_input_text_const() const {
  return input_text;
}

const std::vector<std::string> &TodoViewModel::get_task_entries_const() const {
  return tasks_entries;
}

const std::int32_t &TodoViewModel::get_selected_task_const() const {
  return selected_task;
}

// mutable refs getters
std::list<Task> TodoViewModel::get_tasks() const { return filtered_tasks; }

std::string &TodoViewModel::get_input_text() { return input_text; }

std::vector<std::string> &TodoViewModel::get_task_entries() {
  return tasks_entries;
}

std::int32_t &TodoViewModel::get_seleted_task() { return selected_task; }

//////////////////////////////////////////////////////////////////////////////
// Private helpers
void TodoViewModel::refresh_tasks() {
  filtered_tasks.clear();
  tasks_entries.clear();

  // TODO: add filtering logic

  std::stringstream ss;
  auto tasks = model.getTasks();
  for (const auto &t : tasks) {
    ss << t;
    filtered_tasks.push_back(t);
    // TODO: format the strings nicer
    tasks_entries.push_back(ss.str());

    ss.str(std::string()); // clear the stringstream
  }
}
