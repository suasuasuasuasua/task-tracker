#ifndef INCLUDE_INC_TASK_VIEWMODEL_H_
#define INCLUDE_INC_TASK_VIEWMODEL_H_

#include <filesystem>
#include <ftxui/dom/elements.hpp>
#include <optional>
#include <string>
#include <vector>

#include "task.h"
#include "todo.h"

/**
 * @brief ViewModel for the Task Tracker TUI application.
 *
 * This class bridges the Model (TodoTracker) and View (FTXUI components)
 * following the MVVM pattern. It maintains:
 * - UI state (selected task, current filter, input text)
 * - Commands (add, delete, mark tasks)
 * - Presentation logic (formatting, colors)
 *
 * The View binds to ViewModel state via references and calls ViewModel
 * commands in response to user actions.
 */
class TaskTrackerViewModel {
public:
  /**
   * @brief Construct a new ViewModel and load tasks from file.
   * @param filepath Path to the JSON file for persistence
   */
  explicit TaskTrackerViewModel(const std::filesystem::path &filepath);

  // ==========================================================================
  // Observable State (View reads these)
  // ==========================================================================

  /**
   * @brief Get the list of tasks after applying the current filter.
   * @return Vector of tasks matching the current filter
   */
  const std::vector<Task> &getFilteredTasks() const;

  /**
   * @brief Get the currently selected task index.
   * @return Index into filtered_tasks_
   */
  int getSelectedIndex() const;

  /**
   * @brief Get the current filter status.
   * @return Filter status, or nullopt if showing all tasks
   */
  std::optional<Task::Status> getCurrentFilter() const;

  /**
   * @brief Get the current input text.
   * @return Text in the input field
   */
  const std::string &getInputText() const;

  /**
   * @brief Get the current status message.
   * @return Status/feedback message to display to user
   */
  const std::string &getStatusMessage() const;

  /**
   * @brief Get task entries formatted for Menu component.
   * @return Vector of formatted task strings
   */
  const std::vector<std::string> &getTaskEntries() const;

  // ==========================================================================
  // Mutable State (for two-way binding with FTXUI components)
  // ==========================================================================

  /**
   * @brief Get mutable reference to input text for Input component binding.
   * @return Reference to input text string
   */
  std::string &inputText();

  /**
   * @brief Get mutable reference to selected index for Menu component binding.
   * @return Reference to selected index
   */
  int &selectedIndex();

  /**
   * @brief Get mutable reference to filter selection for Toggle binding.
   * @return Reference to filter index (0=All, 1=Todo, 2=InProgress, 3=Done)
   */
  int &filterIndex();

  // ==========================================================================
  // Commands (View calls these in response to user actions)
  // ==========================================================================

  /**
   * @brief Add a new task with the current input text.
   *
   * Clears input text after adding. Updates status message.
   * Does nothing if input is empty or whitespace-only.
   */
  void addTask();

  /**
   * @brief Delete the currently selected task.
   *
   * Updates status message. Does nothing if no task is selected
   * or list is empty. Adjusts selection if needed.
   */
  void deleteSelectedTask();

  /**
   * @brief Mark the currently selected task with a new status.
   * @param status New status to assign
   */
  void markSelectedTask(Task::Status status);

  /**
   * @brief Update the filter and refresh the filtered task list.
   *
   * Call this when filter_index_ changes (e.g., in Toggle callback).
   */
  void applyFilter();

  /**
   * @brief Save current tasks to the JSON file.
   */
  void save();

  // ==========================================================================
  // Presentation Helpers
  // ==========================================================================

  /**
   * @brief Format a task for display in the task list.
   * @param task Task to format
   * @return Formatted string like "1. Buy groceries [todo]"
   */
  static std::string formatTaskForDisplay(const Task &task);

  /**
   * @brief Get the color associated with a task status.
   * @param status Task status
   * @return FTXUI Color for the status
   */
  static ftxui::Color getStatusColor(Task::Status status);

  /**
   * @brief Get filter option labels for Toggle component.
   *
   * Returns a mutable reference to allow binding to FTXUI Toggle component.
   * The options themselves should not be modified at runtime.
   *
   * @return Mutable reference to filter labels vector
   */
  std::vector<std::string> &filterOptions();

private:
  // Model
  TodoTracker model_;
  std::filesystem::path filepath_;

  // UI State
  std::vector<Task> filtered_tasks_;
  std::vector<std::string> task_entries_; // Formatted strings for Menu
  int selected_index_ = 0;
  int filter_index_ = 0; // 0=All, 1=Todo, 2=InProgress, 3=Done
  std::string input_text_;
  std::string status_message_ = "Ready. Use Enter to add tasks, arrow keys to navigate.";

  // Filter options (mutable to allow FTXUI binding)
  std::vector<std::string> filter_options_ = {"All", "Todo", "In Progress",
                                              "Done"};

  /**
   * @brief Refresh the filtered task list and task entries.
   *
   * Called internally after model changes or filter changes.
   */
  void refreshFilteredTasks();

  /**
   * @brief Get the filter status from filter_index_.
   * @return nullopt for All, or the corresponding Status
   */
  std::optional<Task::Status> getFilterFromIndex() const;
};

#endif // INCLUDE_INC_TASK_VIEWMODEL_H_
