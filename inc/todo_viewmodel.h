#ifndef INCLUDE_INC_TODO_VIEWMODEL_H_
#define INCLUDE_INC_TODO_VIEWMODEL_H_

#include "todo.h"
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
class TodoViewModel {
public:
  TodoViewModel(const std::filesystem::path filepath) : filepath(filepath) {
    model.from_json(filepath);

    // initialize the tasks and task entries
    refresh_tasks();
  }

  ~TodoViewModel() { model.to_json(filepath); }

  //////////////////////////////////////////////////////////////////////////////
  // Model logic
  // setters
  void add_task();

  //////////////////////////////////////////////////////////////////////////////
  // ViewModel logic
  // non-mutable refs getters
  std::list<Task> get_tasks() const;
  const std::string &get_input_text_const() const;
  const std::vector<std::string> &get_task_entries_const() const;
  const std::int32_t &get_selected_task_const() const;

  // mutable refs getters
  std::string &get_input_text();
  std::vector<std::string> &get_task_entries();
  std::int32_t &get_seleted_task();

private:
  // model data (data structures)
  TodoTracker model;
  std::filesystem::path filepath;

  // view model data (ui state)
  std::string input_text;                 // the text for a new task
  std::list<Task> filtered_tasks;         // set of tasks (filter(s) applied)
  std::vector<std::string> tasks_entries; // tasks displayed in the menu
  std::int32_t selected_task;             // the currented selected task

  // helpers
  void refresh_tasks();
};

#endif // INCLUDE_INC_TODO_VIEWMODEL_H_
