#include <filesystem>
#include <iostream>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include "task.h"
#include "todo_viewmodel.h"

// Bring FTXUI names into scope, but Task type is already defined in task.h
using namespace ftxui;
using TaskStatus = ::Task::Status;

namespace {
constexpr const char *logger_name = "logger";
constexpr int modal_min_width = 80;
} // namespace

/// @brief Creates the main task list component
Component MainComponent(TodoViewModel &tvm) {
  auto &ui = tvm.ui_state();
  
  // Task list menu - entries and selection index are managed by UI state
  auto tasklist = Menu(&ui.task_entries(), &ui.selected_index());

  auto component = Container::Vertical({tasklist});

  // Add rendering layer
  component |= Renderer([](Element inner) {
    return window(text("Task Tracker") | center,
                  vbox({window(text("Tasks"), inner)}));
  });

  return component;
}

/// @brief Creates the "Add Task" modal component
Component AddTaskComponent(TodoViewModel &tvm) {
  auto &ui = tvm.ui_state();
  
  auto input_options = InputOption::Spacious();
  input_options.multiline = false;
  input_options.on_enter = [&tvm] { tvm.add_task(); };

  auto text_field = Input(&ui.add_task_input(), input_options);

  auto component = Container::Vertical({text_field});

  component |= Renderer([](Element inner) {
    return vbox({text("Enter the task"), inner}) |
           size(WIDTH, GREATER_THAN, modal_min_width) | border;
  });

  component |= CatchEvent([&ui](Event event) {
    if (event == Event::Escape) {
      ui.hide_add_task_modal();
      return true;
    }
    return false;
  });

  return component;
}

/// @brief Creates the "Edit Task" modal component
Component EditTaskComponent(TodoViewModel &tvm) {
  auto &ui = tvm.ui_state();
  
  auto input_options = InputOption::Spacious();
  input_options.multiline = false;
  input_options.on_enter = [&tvm] { tvm.update_task(); };

  auto text_field = Input(&ui.edit_task_input(), input_options);

  auto component = Container::Vertical({text_field});

  component |= Renderer([](Element inner) {
    return vbox({text("Edit the task"), inner}) |
           size(WIDTH, GREATER_THAN, modal_min_width) | border;
  });

  component |= CatchEvent([&ui](Event event) {
    if (event == Event::Escape) {
      ui.hide_edit_task_modal();
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
  auto &ui = tvm.ui_state();
  spdlog::get(logger_name)
      ->info("Initialized view model from {}", filepath.string());
  spdlog::get(logger_name)->flush();

  // Instanciate the main and modal components:
  auto main_component = MainComponent(tvm);

  main_component |= CatchEvent([&tvm, &ui, &screen](Event event) {
    if (event == Event::d) {
      spdlog::get(logger_name)
          ->debug("Key [d] Deleting selected task: {}.", ui.selected_index());
      tvm.delete_selected_task();
      return true;
    }
    if (event == Event::e) {
      spdlog::get(logger_name)
          ->debug("Key [e] Editing selected task: {}.", ui.selected_index());

      // Fill in the edit field with current task description
      if (auto task = tvm.get_selected_task()) {
        ui.edit_task_input() = task->getDesc();
      }

      ui.show_edit_task_modal();
      return true;
    }
    if (event == Event::i) {
      spdlog::get(logger_name)
          ->debug("Key [i] Marking task in-progress: {}", ui.selected_index());
      tvm.mark_selected_task(TaskStatus::InProgress);
      return true;
    }
    if (event == Event::m) {
      spdlog::get(logger_name)
          ->debug("Key [m] Marking task complete: {}", ui.selected_index());
      tvm.mark_selected_task(TaskStatus::Done);
      return true;
    }
    if (event == Event::n) {
      spdlog::get(logger_name)->debug("Key [n] Showing 'add task' modal.");
      ui.show_add_task_modal();
      return true;
    }
    if (event == Event::q) {
      spdlog::get(logger_name)->debug("Key [q] Quit program requested.");
      screen.ExitLoopClosure()();
      return true;
    }
    if (event == Event::t) {
      spdlog::get(logger_name)
          ->debug("Key [t] Marking task todo: {}", ui.selected_index());
      tvm.mark_selected_task(TaskStatus::ToDo);
      return true;
    }
    return false;
  });

  // Use the `Modal` function to use together the main component and its modal
  // window. The |modal_shown| boolean controls whether the modal is shown or
  // not.
  main_component |=
      Modal(AddTaskComponent(tvm), &ui.add_task_modal_shown());
  main_component |=
      Modal(EditTaskComponent(tvm), &ui.edit_task_modal_shown());
  spdlog::get(logger_name)->debug("Added modal component successfuly.");

  screen.Loop(main_component);
  spdlog::get(logger_name)->flush();

  spdlog::get(logger_name)->info("Closing TUI successfully.\n");
  return 0;
}
