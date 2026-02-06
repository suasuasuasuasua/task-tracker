#include <argparse/argparse.hpp>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>
#include <list>
#include <nlohmann/json.hpp>
#include <string>

#include "task.h"
#include "todo.h"

#ifndef VERSION
#define VERSION "unknown version"
#endif

using json = nlohmann::json;

const std::string version = VERSION;
bool verbose_flag = false;

int main(int argc, char *argv[]) {
  // create the todo list under home if possible
  std::filesystem::path filepath = "todo.json";
  if (auto home_dir = std::getenv("HOME"); home_dir != nullptr) {
    filepath = home_dir / filepath;
  }

  argparse::ArgumentParser program("task-cli", version);
  program.add_argument("--verbose")
      .help("enable verbose mode")
      .flag()
      .store_into(verbose_flag);
  program.add_argument("-p", "--path")
      .help("The path of the data file")
      .default_value(filepath)
      .store_into(filepath);

  // add subparser
  argparse::ArgumentParser add_command("add");
  add_command.add_description("add new task");
  add_command.add_argument("task").help("The new task to add").remaining();

  // update subparser
  argparse::ArgumentParser update_command("update");
  update_command.add_description("update a task");
  update_command.add_argument("id").scan<'u', std::uint32_t>();
  update_command.add_argument("desc").help(
      "The updated description of the task");

  // delete subparser
  argparse::ArgumentParser delete_command("delete");
  delete_command.add_description("delete a task");
  delete_command.add_argument("id")
      .help("The id of the task to be deleted")
      .scan<'u', std::uint32_t>();

  // mark subparser
  argparse::ArgumentParser mark_command("mark");
  mark_command.add_description("mark a task");
  mark_command.add_argument("id")
      .help("The id of the task to be marked")
      .scan<'u', std::uint32_t>();
  mark_command.add_argument("status")
      .help("The status to mark the task")
      .choices("d",
               "done"
               "i",
               "in-progress", "t", "todo");

  // list subparser
  argparse::ArgumentParser list_command("list");
  list_command.add_description("list all tasks");
  list_command.add_argument("-f", "--filter")
      .help("The status to filer the tasks by")
      .choices("d",
               "done"
               "i",
               "in-progress", "none", "t", "todo")
      .default_value("none");

  program.add_subparser(add_command);
  program.add_subparser(update_command);
  program.add_subparser(delete_command);
  program.add_subparser(mark_command);
  program.add_subparser(list_command);

  // Parse the arguments
  try {
    program.parse_args(argc, argv);
  } catch (const std::exception &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  // Data file path
  if (filepath.extension() != ".json") {
    std::cerr << "Extension must be .json\n";
    std::exit(1);
  } else if (filepath.empty()) {
    std::cerr << "File path cannot be empty";
    std::exit(1);
  }

  TodoTracker todolist;
  todolist.from_json(filepath);

  // Add
  if (program.is_subcommand_used("add")) {
    try {
      auto tasks = add_command.get<std::list<std::string>>("task");
      for (const auto &task : tasks) {
        auto uid = todolist.add_task(task);
        std::cout << std::format("Task added successfully (ID: {})\n", uid);
      }
    } catch (std::logic_error &e) {
      std::cout << "No files provided: " << e.what() << std::endl;
    }
  }

  // Update
  if (program.is_subcommand_used("update")) {
    try {
      auto uid = update_command.get<std::uint32_t>("id");
      auto desc = update_command.get<std::string>("desc");
      todolist.update_task(uid, desc);
      std::cout << std::format("Task updated successfully (ID: {})\n", uid);
    } catch (std::logic_error &e) {
      std::cout << "Something went wrong: " << e.what() << std::endl;
    }
  }

  // Delete
  if (program.is_subcommand_used("delete")) {
    try {
      auto uid = delete_command.get<std::uint32_t>("id");
      todolist.delete_task(uid);
      std::cout << std::format("Task deleted successfully (ID: {})\n", uid);
    } catch (std::logic_error &e) {
      std::cout << "Something went wrong: " << e.what() << std::endl;
    }
  }

  // Mark
  if (program.is_subcommand_used("mark")) {
    try {
      auto uid = mark_command.get<std::uint32_t>("id");
      auto stat = mark_command.get<std::string>("status");
      Task::Status status = Task::String2Stat.at(stat);
      todolist.mark_task(uid, status);
      std::cout << std::format("Task marked successfully (ID: {})\n", uid);
    } catch (std::logic_error &e) {
      std::cout << "Something went wrong: " << e.what() << std::endl;
    }
  }

  // List
  if (program.is_subcommand_used("list")) {
    try {
      auto filter_str = list_command.get("-f");
      std::optional<Task::Status> status;
      if (filter_str == "none") {
        status = std::nullopt;
      } else {
        status = Task::String2Stat.at(filter_str);
      }
      todolist.list_tasks(status);
    } catch (std::logic_error &e) {
      std::cout << "Something went wrong: " << e.what() << std::endl;
    }
  }

  todolist.to_json(filepath);
}
