#include <filesystem>
#include <format>
#include <functional>
#include <iostream>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include "task.h"
#include "todo_viewmodel.h"

using namespace ftxui;

constexpr std::string logger_name = "logger";

Component MainComponent(TodoViewModel &tvm) {
  // define the menulist of task items
  // the entries are defined via const reference, and the index is a mutable ref
  auto tasklist = Menu(&tvm.get_task_entries_const(), &tvm.get_selected_task());

  auto button_style = ButtonOption::Simple();
  auto component = Container::Vertical({
      Button("Add Task", tvm.addtask_show(), button_style),
      tasklist,
      // TODO: add exit button back with callback. should it be in viewmodel?
      // Button("Quit", exit, button_style),
  });

  // Polish how the two buttons are rendered:
  component |= Renderer([&](Element inner) {
    Elements elems = {inner, separator()};

    if (not tvm.get_tasks().empty()) {
      auto entries = tvm.get_task_entries_const();
      auto idx = tvm.get_selected_task_const();
      elems.emplace_back(
          text(std::format("Current selected text: {}", entries.at(idx))));
      elems.emplace_back(text(std::format("Current selected input: {}", idx)));
    }

    return window(text("Task Tracker") | bold | center, vbox(elems));
  });

  return component;
}

Component AddTaskComponent(TodoViewModel &tvm) {

  auto input_options = InputOption::Spacious();
  input_options.multiline = false;
  input_options.on_enter = [&tvm] { tvm.add_task(); };

  auto text_field =
      Input(&tvm.get_input_text(), "Enter a task!", input_options);

  auto button_style = ButtonOption::Simple();
  auto component = Container::Vertical({
      text_field,
      Button("Quit", tvm.addtask_hide(), button_style),
  });
  // Polish how the two buttons are rendered:
  component |= Renderer([&](Element inner) {
    return vbox({
               text("Enter the task"),
               separator(),
               inner,
           }) |
           size(WIDTH, GREATER_THAN, 80) | border;
  });

  component |= CatchEvent([&tvm](Event event) {
    if (event == Event::Escape) {
      tvm.addtask_hide()();
      return true;
    }
    return false;
  });

  return component;
}

int main(int argc, const char *argv[]) {
  // Find the user's home directory
  std::filesystem::path home_dir;
  if (auto home_env = std::getenv("HOME"); home_env != nullptr) {
    home_dir = home_env;
  } else {
    // Fallback to current directory if HOME is not set
    home_dir = std::filesystem::current_path();
  }

  // Setup the loggers
  spdlog::set_level(spdlog::level::debug); // Set *global* log level to debug
  try {
    // TODO: parameterize this path to XDG_STATE_DIR or the like
    auto log_dir = home_dir / ".local/state/task-tracker";
    std::filesystem::create_directories(log_dir);
    auto log_path = log_dir / "log.txt";
    auto logger = spdlog::basic_logger_mt(logger_name, log_path);
    // flush logs every few seconds
    spdlog::flush_every(std::chrono::seconds(3));
  } catch (const spdlog::spdlog_ex &ex) {
    std::cerr << "Log init failed: " << ex.what() << std::endl;
  }
  spdlog::get(logger_name)->debug("Initialized logger successfully.");

  // Setup the screen
  auto screen = ScreenInteractive::Fullscreen();
  spdlog::get(logger_name)->debug("Initialized screen successfully.");

  // TODO: add this shared routine to a util file
  // create the todo list under home if possible
  std::string filename = "todo.json";
  std::filesystem::path filepath = home_dir / filename;
  spdlog::get(logger_name)->info("Reading from {}", filepath.string());

  // Create the view model that will mediate data between the models and views
  TodoViewModel tvm(filepath);
  spdlog::get(logger_name)
      ->info("Initialized view model from {}", filepath.string());
  spdlog::get(logger_name)->flush();

  // Instanciate the main and modal components:
  auto main_component = MainComponent(tvm);
  auto add_task_component = AddTaskComponent(tvm);

  main_component |= CatchEvent([&tvm, &screen](Event event) {
    if (event == Event::d) {
      spdlog::get(logger_name)
          ->debug("Key [d] Deleting selected task: {}.",
                  tvm.get_selected_task_const());
      tvm.delete_selected_task();
      return true;
    }
    if (event == Event::i) {
      spdlog::get(logger_name)
          ->debug("Key [i] Marking task in-progress: {}",
                  tvm.get_selected_task_const());
      tvm.mark_task(::Task::Status::InProgress);
      return true;
    }
    if (event == Event::m) {
      spdlog::get(logger_name)
          ->debug("Key [m] Marking task complete: {}",
                  tvm.get_selected_task_const());
      tvm.mark_task(::Task::Status::Done);
      return true;
    }
    if (event == Event::n) {
      spdlog::get(logger_name)->debug("Key [n] Showing 'add task' modal.");
      tvm.addtask_show()();
      return true;
    }
    if (event == Event::q) {
      spdlog::get(logger_name)->debug("Key [q] Quit program requested.");
      screen.ExitLoopClosure()();
      return true;
    }
    if (event == Event::t) {
      spdlog::get(logger_name)
          ->debug("Key [t] Marking task todo: {}",
                  tvm.get_selected_task_const());
      tvm.mark_task(::Task::Status::ToDo);
      return true;
    }
    return false;
  });

  // Use the `Modal` function to use together the main component and its modal
  // window. The |modal_shown| boolean controls whether the modal is shown or
  // not.
  main_component |= Modal(add_task_component, &tvm.get_addtask_input_shown());
  spdlog::get(logger_name)->debug("Added modal component successfuly.");

  screen.Loop(main_component);
  spdlog::get(logger_name)->flush();

  spdlog::get(logger_name)->info("Closing TUI successfully.\n");
  return 0;
}
