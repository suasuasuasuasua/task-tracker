#ifndef INCLUDE_INC_TODO_VIEWMODEL_H_
#define INCLUDE_INC_TODO_VIEWMODEL_H_

#include "todo.h"
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
class TodoViewModel {
public:
  TodoViewModel(const std::filesystem::path filepath)
      : filepath(filepath), addtask_input_shown(false) {
    model.from_json(filepath);

    // initialize the tasks and task entries
    refresh_tasks();
  }

  ~TodoViewModel() { model.to_json(filepath); }

  //////////////////////////////////////////////////////////////////////////////
  // Model logic
  void add_task();
  void delete_selected_task();

  //////////////////////////////////////////////////////////////////////////////
  // ViewModel logic
  // non-mutable refs getters
  std::vector<Task> get_tasks() const;
  const std::string &get_input_text_const() const;
  const std::vector<std::string> &get_task_entries_const() const;
  const std::int32_t &get_selected_task_const() const;
  const bool &get_addtask_input_shown() const;

  // mutable refs getters
  std::string &get_input_text();
  std::vector<std::string> &get_task_entries();
  std::int32_t &get_selected_task();

  //////////////////////////////////////////////////////////////////////////////
  // View logic
  // callbacks (closures)
  std::function<void()> addtask_show();
  std::function<void()> addtask_hide();

private:
  // model data (data structures)
  TodoTracker model;
  std::filesystem::path filepath;

  // view model data (ui state)
  std::string input_text;                 // the text for a new task
  std::vector<Task> filtered_tasks;       // set of tasks (filter(s) applied)
  std::vector<std::string> tasks_entries; // tasks displayed in the menu
  std::int32_t selected_task_idx;         // the currented selected task

  bool addtask_input_shown;

  // helpers
  void refresh_tasks();
};

#endif // INCLUDE_INC_TODO_VIEWMODEL_H_
