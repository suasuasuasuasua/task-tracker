/**
 * @file tui.cpp
 * @brief Task Tracker TUI application using FTXUI and MVVM pattern.
 *
 * This file implements the View layer of the MVVM architecture.
 * It binds FTXUI components to the TaskTrackerViewModel and handles
 * user input events.
 *
 * Architecture:
 * - Model: Task, TodoTracker (data and persistence)
 * - ViewModel: TaskTrackerViewModel (UI state, commands, presentation logic)
 * - View: This file (FTXUI components and rendering)
 *
 * See docs/architecture.md for detailed design documentation.
 */

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

#include <filesystem>
#include <iostream>
#include <string>

#include "task_viewmodel.h"

using namespace ftxui;

/**
 * @brief Get the file path for storing task data.
 * @return Path to todo.json in home directory or current directory
 */
std::filesystem::path getTaskFilePath() {
  const std::string filename = "todo.json";
  if (auto home_dir = std::getenv("HOME"); home_dir != nullptr) {
    return std::filesystem::path(home_dir) / filename;
  }
  return filename;
}

/**
 * @brief Create an event handler for quitting the application.
 * @param screen Reference to the screen to exit
 * @return Component decorator that catches 'q' and 'Q' keys
 */
ComponentDecorator quitHandler(ScreenInteractive &screen) {
  return CatchEvent([&screen](Event event) {
    if (event == Event::Character('q') || event == Event::Character('Q')) {
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });
}

/**
 * @brief Create the main event handler for task operations.
 * @param viewmodel Reference to the ViewModel
 * @param screen Reference to the screen (for exiting)
 * @return Component decorator that handles all keyboard shortcuts
 */
ComponentDecorator mainEventHandler(TaskTrackerViewModel &viewmodel,
                                    ScreenInteractive &screen) {
  return CatchEvent([&viewmodel, &screen](Event event) {
    // Quit
    if (event == Event::Character('q') || event == Event::Character('Q')) {
      screen.ExitLoopClosure()();
      return true;
    }

    // Delete selected task
    if (event == Event::Character('d') || event == Event::Character('D')) {
      viewmodel.deleteSelectedTask();
      return true;
    }

    // Mark as done
    if (event == Event::Character('m') || event == Event::Character('M')) {
      viewmodel.markSelectedTask(Task::Status::Done);
      return true;
    }

    // Mark as in-progress
    if (event == Event::Character('i') || event == Event::Character('I')) {
      viewmodel.markSelectedTask(Task::Status::InProgress);
      return true;
    }

    // Mark as todo
    if (event == Event::Character('t') || event == Event::Character('T')) {
      viewmodel.markSelectedTask(Task::Status::ToDo);
      return true;
    }

    // Filter shortcuts (1-4)
    if (event == Event::Character('1')) {
      viewmodel.filterIndex() = 0; // All
      viewmodel.applyFilter();
      return true;
    }
    if (event == Event::Character('2')) {
      viewmodel.filterIndex() = 1; // Todo
      viewmodel.applyFilter();
      return true;
    }
    if (event == Event::Character('3')) {
      viewmodel.filterIndex() = 2; // In Progress
      viewmodel.applyFilter();
      return true;
    }
    if (event == Event::Character('4')) {
      viewmodel.filterIndex() = 3; // Done
      viewmodel.applyFilter();
      return true;
    }

    return false;
  });
}

/**
 * @brief Build the help text element.
 * @return Element displaying keyboard shortcuts
 */
Element buildHelpText() {
  return hbox({
             text("Enter=add"),
             text(" | "),
             text("(d)elete") | color(Color::Red),
             text(" | "),
             text("(m)ark done") | color(Color::Green),
             text(" | "),
             text("(i)n-progress") | color(Color::Blue),
             text(" | "),
             text("(t)odo") | color(Color::Yellow),
             text(" | "),
             text("(1-4) filter"),
             text(" | "),
             text("(q)uit"),
         }) |
         dim;
}

/**
 * @brief Build a styled task element with status coloring.
 * @param task Task to display
 * @param is_selected Whether this task is currently selected
 * @return Styled Element for the task
 */
Element buildTaskElement(const Task &task, bool is_selected) {
  auto status_color = TaskTrackerViewModel::getStatusColor(task.getStatus());
  auto status_text =
      text("[" + Task::Stat2String.at(task.getStatus()) + "]") |
      color(status_color);

  auto task_text =
      hbox({text(std::to_string(task.getUid()) + ". "), text(task.getDesc()),
            filler(), status_text});

  if (is_selected) {
    task_text = task_text | inverted;
  }

  return task_text;
}

int main() {
  // Initialize ViewModel with persistence path
  auto filepath = getTaskFilePath();
  TaskTrackerViewModel viewmodel(filepath);

  // Create screen
  auto screen = ScreenInteractive::Fullscreen();

  // ==========================================================================
  // Create FTXUI Components (bound to ViewModel state)
  // ==========================================================================

  // Input component for new tasks
  InputOption input_option;
  input_option.on_enter = [&viewmodel] { viewmodel.addTask(); };
  input_option.multiline = false;
  auto input_component =
      Input(&viewmodel.inputText(), "Enter new task...", input_option);

  // Add button
  auto add_button = Button(
      "Add", [&viewmodel] { viewmodel.addTask(); }, ButtonOption::Animated());

  // Task list menu
  MenuOption menu_option;
  menu_option.on_change = [&viewmodel] {
    // Called when selection changes - could add preview functionality here
  };
  auto task_menu = Menu(&viewmodel.getTaskEntries(), &viewmodel.selectedIndex(),
                        menu_option);

  // Filter toggle with on_change callback to apply filter
  ToggleOption toggle_option;
  toggle_option.on_change = [&viewmodel] { viewmodel.applyFilter(); };
  auto filter_toggle =
      Toggle(&viewmodel.filterOptions(), &viewmodel.filterIndex(), toggle_option);

  // ==========================================================================
  // Create Container (defines focus navigation)
  // ==========================================================================

  auto container = Container::Vertical({
      Container::Horizontal({input_component, add_button}),
      filter_toggle,
      task_menu,
  });

  // ==========================================================================
  // Create Renderer (builds the UI from ViewModel state)
  // ==========================================================================

  auto renderer = Renderer(container, [&] {
    // Build task list elements with custom styling
    Elements task_elements;
    const auto &tasks = viewmodel.getFilteredTasks();

    if (tasks.empty()) {
      task_elements.push_back(
          text("No tasks. Add one above!") | dim | center);
    } else {
      for (size_t i = 0; i < tasks.size(); ++i) {
        bool is_selected = (static_cast<int>(i) == viewmodel.getSelectedIndex());
        task_elements.push_back(buildTaskElement(tasks[i], is_selected));
      }
    }

    // Build filter label with current filter highlighted
    auto filter_label = hbox({
        text("Filter: "),
        filter_toggle->Render(),
    });

    // Build the complete UI
    return window(
        text("Task Tracker") | bold | center,
        vbox({
            // Input row
            hbox({
                text("New: "),
                input_component->Render() | flex,
                text(" "),
                add_button->Render(),
            }),
            separator(),

            // Filter row
            filter_label,
            separator(),

            // Task list (scrollable)
            vbox(task_elements) | frame | flex,
            separator(),

            // Help text
            buildHelpText(),

            // Status message
            text(viewmodel.getStatusMessage()) |
                color(Color::Cyan),
        }));
  });

  // ==========================================================================
  // Add Event Handlers
  // ==========================================================================

  renderer |= mainEventHandler(viewmodel, screen);

  // ==========================================================================
  // Run the Application
  // ==========================================================================

  screen.Loop(renderer);

  // Save on exit
  try {
    viewmodel.save();
  } catch (const std::exception &e) {
    // In a production app, you might want to show an error dialog
    // For now, we print to stderr since the TUI has already exited
    std::cerr << "Error saving tasks: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

