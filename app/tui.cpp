#include <filesystem>
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

  // list of components in the main screen
  auto component = Container::Vertical({tasklist});

  // add a renderer to the main screen
  component |= Renderer([&](Element inner) {
    Elements elems = {
        window(text("Tasks"), inner),
    };

    return window(text("Task Tracker") | center, vbox(elems));
  });

  return component;
}

Component AddTaskComponent(TodoViewModel &tvm) {
  auto input_options = InputOption::Spacious();
  input_options.multiline = false;
  input_options.on_enter = [&tvm] { tvm.add_task(); };

  auto text_field =
      Input(&tvm.get_input_text(), "Enter a task!", input_options);

  auto component = Container::Vertical({
      text_field,
  });

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

Component EditTaskComponent(TodoViewModel &tvm) {
  auto input_options = InputOption::Spacious();
  input_options.multiline = false;
  input_options.on_enter = [&tvm] { tvm.update_task(); };

  auto text_field =
      Input(&tvm.get_input_text(), "Edit this task!", input_options);

  auto component = Container::Vertical({
      text_field,
  });

  component |= Renderer([&](Element inner) {
    return vbox({
               text("Edit the task"),
               separator(),
               inner,
           }) |
           size(WIDTH, GREATER_THAN, 80) | border;
  });

  component |= CatchEvent([&tvm](Event event) {
    if (event == Event::Escape) {
      tvm.edittask_hide()();
      return true;
    }
    return false;
  });

  return component;
}

int main(int argc, const char *argv[]) {
  // Safely get home directory - use current directory as fallback
  std::filesystem::path home_dir;
  if (auto home_env = std::getenv("HOME"); home_env != nullptr) {
    home_dir = home_env;
  } else {
    home_dir = std::filesystem::current_path();
  }

  // Setup the loggers
  spdlog::set_level(spdlog::level::debug); // Set *global* log level to debug
  // Use XDG_STATE_HOME if available, otherwise fallback to $HOME/.local/state
  std::filesystem::path state_dir;
  try {
    if (auto xdg_state = std::getenv("XDG_STATE_HOME"); xdg_state != nullptr) {
      state_dir = xdg_state;
    } else {
      state_dir = home_dir / ".local/state";
    }
    auto log_path = state_dir / "task-tracker/log.txt";

    // setup the logger object
    auto logger = spdlog::basic_logger_mt(logger_name, log_path);
    // flush logs every few seconds
    spdlog::flush_every(std::chrono::seconds(3));
  } catch (const spdlog::spdlog_ex &ex) {
    std::cerr << "Log init failed: " << ex.what() << std::endl;
  }
  spdlog::get(logger_name)
      ->debug("Initialized logger sucessfully. Writing logs to {}.",
              state_dir.string());

  // Setup the screen
  auto screen = ScreenInteractive::Fullscreen();
  spdlog::get(logger_name)->debug("Initialized screen sucessfully.");

  // create the todo list under home if possible
  std::string filename = "todo.json";
  std::filesystem::path filepath = home_dir / filename;
  spdlog::get(logger_name)->info("Reading data from {}", filepath.string());

  // Create the view model that will mediate data between the models and views
  TodoViewModel tvm(filepath);
  spdlog::get(logger_name)
      ->info("Initialized view model from {}", filepath.string());
  spdlog::get(logger_name)->flush();

  // Instanciate the main and modal components:
  auto main_component = MainComponent(tvm);

  main_component |= CatchEvent([&tvm, &screen](Event event) {
    if (event == Event::d) {
      spdlog::get(logger_name)
          ->debug("Key [d] Deleting selected task: {}.",
                  tvm.get_selected_task_const());
      tvm.delete_selected_task();
      return true;
    }
    if (event == Event::e) {
      spdlog::get(logger_name)
          ->debug("Key [e] Editing selected task: {}.",
                  tvm.get_selected_task_const());

      // fill in the edit field
      if (not tvm.get_tasks().empty()) {
        tvm.get_input_text() =
            tvm.get_tasks().at(tvm.get_selected_task_const()).getDesc();
      }

      tvm.edittask_show()();
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
  main_component |=
      Modal(AddTaskComponent(tvm), &tvm.get_addtask_input_shown());
  main_component |=
      Modal(EditTaskComponent(tvm), &tvm.get_edittask_input_shown());
  spdlog::get(logger_name)->debug("Added modal component successfuly.");

  screen.Loop(main_component);
  spdlog::get(logger_name)->flush();

  spdlog::get(logger_name)->info("Closing TUI successfully.\n");
  return 0;
}
