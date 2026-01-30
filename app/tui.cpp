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
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

#include <filesystem>
#include <iostream>
#include <string>

#include "task_viewmodel.h"

// Note: We avoid "using namespace ftxui;" because ftxui::Task conflicts with
// our Task class from task.h. Instead, we use explicit ftxui:: prefixes.

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
 * @brief Create the main event handler for task operations.
 * @param viewmodel Reference to the ViewModel
 * @param screen Reference to the screen (for exiting)
 * @param input_component Reference to input component to check focus state
 * @return Component decorator that handles all keyboard shortcuts
 *
 * Note: Keyboard shortcuts are only active when the input field is NOT focused,
 * allowing normal text entry in the input field.
 */
ftxui::ComponentDecorator mainEventHandler(TaskTrackerViewModel &viewmodel,
                                    ftxui::ScreenInteractive &screen,
                                    ftxui::Component &input_component) {
  return ftxui::CatchEvent([&viewmodel, &screen, &input_component](ftxui::Event event) {
    // If input is focused, don't intercept character events
    // This allows typing in the input field without triggering shortcuts
    if (input_component->Focused() && event.is_character()) {
      return false; // Let the input component handle the character
    }

    // Quit
    if (event == ftxui::Event::Character('q') || event == ftxui::Event::Character('Q')) {
      screen.ExitLoopClosure()();
      return true;
    }

    // Delete selected task
    if (event == ftxui::Event::Character('d') || event == ftxui::Event::Character('D')) {
      viewmodel.deleteSelectedTask();
      return true;
    }

    // Mark as done
    if (event == ftxui::Event::Character('m') || event == ftxui::Event::Character('M')) {
      viewmodel.markSelectedTask(Task::Status::Done);
      return true;
    }

    // Mark as in-progress
    if (event == ftxui::Event::Character('i') || event == ftxui::Event::Character('I')) {
      viewmodel.markSelectedTask(Task::Status::InProgress);
      return true;
    }

    // Mark as todo
    if (event == ftxui::Event::Character('t') || event == ftxui::Event::Character('T')) {
      viewmodel.markSelectedTask(Task::Status::ToDo);
      return true;
    }

    // Filter shortcuts (1-4)
    if (event == ftxui::Event::Character('1')) {
      viewmodel.filterIndex() = 0; // All
      viewmodel.applyFilter();
      return true;
    }
    if (event == ftxui::Event::Character('2')) {
      viewmodel.filterIndex() = 1; // Todo
      viewmodel.applyFilter();
      return true;
    }
    if (event == ftxui::Event::Character('3')) {
      viewmodel.filterIndex() = 2; // In Progress
      viewmodel.applyFilter();
      return true;
    }
    if (event == ftxui::Event::Character('4')) {
      viewmodel.filterIndex() = 3; // Done
      viewmodel.applyFilter();
      return true;
    }

    return false;
  });
}

/**
 * @brief Build the help text element.
 * @return Element displaying keyboard shortcuts (active when not typing)
 */
ftxui::Element buildHelpText() {
  return ftxui::hbox({
             ftxui::text("Enter=add"),
             ftxui::text(" | "),
             ftxui::text("(d)elete") | ftxui::color(ftxui::Color::Red),
             ftxui::text(" | "),
             ftxui::text("(m)ark done") | ftxui::color(ftxui::Color::Green),
             ftxui::text(" | "),
             ftxui::text("(i)n-progress") | ftxui::color(ftxui::Color::Blue),
             ftxui::text(" | "),
             ftxui::text("(t)odo") | ftxui::color(ftxui::Color::Yellow),
             ftxui::text(" | "),
             ftxui::text("(1-4) filter"),
             ftxui::text(" | "),
             ftxui::text("(q)uit"),
         }) |
         ftxui::dim;
}

/**
 * @brief Build a styled task element with status coloring.
 * @param task Task to display
 * @param is_selected Whether this task is currently selected
 * @return Styled Element for the task
 */
ftxui::Element buildTaskElement(const Task &task, bool is_selected) {
  auto status_color = TaskTrackerViewModel::getStatusColor(task.getStatus());
  auto status_text =
      ftxui::text("[" + Task::Stat2String.at(task.getStatus()) + "]") |
      ftxui::color(status_color);

  auto task_text =
      ftxui::hbox({ftxui::text(std::to_string(task.getUid()) + ". "), ftxui::text(task.getDesc()),
            ftxui::filler(), status_text});

  if (is_selected) {
    task_text = task_text | ftxui::inverted;
  }

  return task_text;
}

int main() {
  // Initialize ViewModel with persistence path
  auto filepath = getTaskFilePath();
  TaskTrackerViewModel viewmodel(filepath);

  // Create screen
  auto screen = ftxui::ScreenInteractive::Fullscreen();

  // ==========================================================================
  // Create FTXUI Components (bound to ViewModel state)
  // ==========================================================================

  // Input component for new tasks
  ftxui::InputOption input_option;
  input_option.on_enter = [&viewmodel] { viewmodel.addTask(); };
  input_option.multiline = false;
  auto input_component =
      ftxui::Input(&viewmodel.inputText(), "Enter new task...", input_option);

  // Add button
  auto add_button = ftxui::Button(
      "Add", [&viewmodel] { viewmodel.addTask(); }, ftxui::ButtonOption::Animated());

  // Task list menu
  ftxui::MenuOption menu_option;
  menu_option.on_change = [&viewmodel] {
    // Called when selection changes - could add preview functionality here
  };
  auto task_menu = ftxui::Menu(&viewmodel.getTaskEntries(), &viewmodel.selectedIndex(),
                        menu_option);

  // Filter toggle with on_change callback to apply filter
  ftxui::ToggleOption toggle_option;
  toggle_option.on_change = [&viewmodel] { viewmodel.applyFilter(); };
  auto filter_toggle =
      ftxui::Toggle(&viewmodel.filterOptions(), &viewmodel.filterIndex(), toggle_option);

  // ==========================================================================
  // Create Container (defines focus navigation)
  // ==========================================================================

  auto container = ftxui::Container::Vertical({
      ftxui::Container::Horizontal({input_component, add_button}),
      filter_toggle,
      task_menu,
  });

  // ==========================================================================
  // Create Renderer (builds the UI from ViewModel state)
  // ==========================================================================

  auto renderer = ftxui::Renderer(container, [&] {
    // Build task list elements with custom styling
    ftxui::Elements task_elements;
    const auto &tasks = viewmodel.getFilteredTasks();

    if (tasks.empty()) {
      task_elements.push_back(
          ftxui::text("No tasks. Add one above!") | ftxui::dim | ftxui::center);
    } else {
      for (size_t i = 0; i < tasks.size(); ++i) {
        bool is_selected = (static_cast<int>(i) == viewmodel.getSelectedIndex());
        task_elements.push_back(buildTaskElement(tasks[i], is_selected));
      }
    }

    // Build filter label with current filter highlighted
    auto filter_label = ftxui::hbox({
        ftxui::text("Filter: "),
        filter_toggle->Render(),
    });

    // Build the complete UI
    return ftxui::window(
        ftxui::text("Task Tracker") | ftxui::bold | ftxui::center,
        ftxui::vbox({
            // Input row
            ftxui::hbox({
                ftxui::text("New: "),
                input_component->Render() | ftxui::flex,
                ftxui::text(" "),
                add_button->Render(),
            }),
            ftxui::separator(),

            // Filter row
            filter_label,
            ftxui::separator(),

            // Task list (scrollable)
            ftxui::vbox(task_elements) | ftxui::frame | ftxui::flex,
            ftxui::separator(),

            // Help text
            buildHelpText(),

            // Status message
            ftxui::text(viewmodel.getStatusMessage()) |
                ftxui::color(ftxui::Color::Cyan),
        }));
  });

  // ==========================================================================
  // Add Event Handlers
  // ==========================================================================

  renderer |= mainEventHandler(viewmodel, screen, input_component);

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

