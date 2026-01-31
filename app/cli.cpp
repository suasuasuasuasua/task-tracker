#include <cstdint>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "task.h"
#include "todo.h"

using json = nlohmann::json;

int main(int argc, char *argv[]) {
  const std::string help_str =
      "add, delete, update, mark-in-progress, mark-done, list, list "
      "done, list todo, list in-progress, help, exit \n";

  // handle empty call
  if (argc == 1) {
    std::cout << help_str;
    return 0;
  }

  std::string filename = "todo.json";
  std::filesystem::path filepath;
  // create the todo list under home if possible
  if (auto home_dir = std::getenv("HOME"); home_dir != nullptr) {
    filepath = filepath / home_dir / filename;
  }
  // else create under local directory
  else {
    filepath = filename;
  }

  TodoTracker todolist;
  todolist.from_json(filepath);

  // convert argv to strings vector for ease of use
  std::vector<std::string> argv_s(argv + 1, argv + argc);
  // handle subcommands
  std::string subcommand = argv_s.front();
  auto rest = std::vector<std::string>(argv_s.begin() + 1, argv_s.end());

  if (subcommand == "help") {
    std::cout << help_str;
  } else if (subcommand == "add" and rest.size() == 1) {
    auto desc = rest.front();
    if (desc.empty()) {
      std::cerr << "Error: Task description cannot be empty\n";
      return 1;
    }
    auto uid = todolist.add_task(desc);

    std::cout << "Task added successfully (ID: " << uid << ")\n";
  } else if (subcommand == "update" and rest.size() == 2) {
    try {
      std::uint32_t uid = std::stoul(rest.at(0));
      std::string desc = rest.at(1);
      if (desc.empty()) {
        std::cerr << "Error: Task description cannot be empty\n";
        return 1;
      }

      todolist.update_task(uid, desc);
      std::cout << "Task updated successfully (ID: " << uid << ")\n";
    } catch (const std::exception &e) {
      std::cerr << "Error: Invalid task ID\n";
      return 1;
    }
  } else if (subcommand == "delete" and rest.size() == 1) {
    try {
      std::uint32_t uid = std::stoul(rest.front());

      todolist.delete_task(uid);
      std::cout << "Task deleted successfully (ID: " << uid << ")\n";
    } catch (const std::exception &e) {
      std::cerr << "Error: Invalid task ID\n";
      return 1;
    }
  } else if (subcommand.find("mark") != std::string::npos and
             rest.size() == 1) {
    try {
      std::uint32_t uid = std::stoul(rest.front());
      Task::Status status;

    if (subcommand == "mark-in-progress") {
      status = Task::Status::InProgress;
    } else if (subcommand == "mark-done") {
      status = Task::Status::Done;
    } else {
      std::cout << "Unrecognized command\n";
      return 1;
    }

    todolist.mark_task(uid, status);
    std::cout << "Task status updated successfully (ID: " << uid << ")\n";
    } catch (const std::exception &e) {
      std::cerr << "Error: Invalid task ID\n";
      return 1;
    }
  } else if (subcommand == "list") {
    switch (rest.size()) {
    // print all the tasks
    case 0:
      // TODO: rework so we don't need three args
      todolist.list_tasks(Task::Status::ToDo, false);
      break;
    // print all the tasks with some filter
    case 1: {
      auto filt = rest.front();

      // check if the filter is valid
      auto it = Task::String2Stat.find(filt);
      if (it == Task::String2Stat.end()) {
        std::cout << "Could not find type: " << filt << "\n";
        return 1;
      }

      auto status = it->second;
      todolist.list_tasks(status);
    } break;
    default:
      std::cout << "Unrecognized command";
      return 1;
      break;
    }
  } else {
    std::cout << "Unrecognized commands!\n";
    return 1;
  }

  todolist.to_json(filepath);

  return 0;
}
