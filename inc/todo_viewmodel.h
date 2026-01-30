#ifndef INCLUDE_INC_TODO_VIEWMODEL_H_
#define INCLUDE_INC_TODO_VIEWMODEL_H_

#include "todo.h"
#include <filesystem>
#include <string>
class TodoViewModel {
public:
  TodoViewModel(const std::filesystem::path filepath) : filepath(filepath) {
    model.from_json(filepath);

    current_tasks = model.getTasks();
  }

  std::list<Task> get_tasks();

  /**
   * @brief Get the data of the input text [non-mutable]
   */
  const std::string &get_input_text_const() const;
  /**
   * @brief get the reference to the input text data [mutable]
   */
  std::string &get_input_text();

private:
  // model data (data structures)
  TodoTracker model;
  std::filesystem::path filepath;

  // view model data (ui state)
  std::string input_text;
  std::list<Task> current_tasks;
};

#endif // INCLUDE_INC_TODO_VIEWMODEL_H_
