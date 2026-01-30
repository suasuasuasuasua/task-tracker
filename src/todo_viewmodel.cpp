#include "todo_viewmodel.h"

std::list<Task> TodoViewModel::get_tasks() { return current_tasks; }

const std::string &TodoViewModel::get_input_text_const() const {
  return input_text;
}

std::string &TodoViewModel::get_input_text() { return input_text; }
