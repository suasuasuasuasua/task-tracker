# Architecture

This document describes the architecture and design patterns for the Task Tracker TUI application using C++20 and the FTXUI library.

## Table of Contents

1. [Overview](#overview)
2. [MVC vs MVVM](#mvc-vs-mvvm)
3. [Recommended Pattern: MVVM](#recommended-pattern-mvvm)
4. [Architecture Diagram](#architecture-diagram)
5. [Component Breakdown](#component-breakdown)
6. [Data Flow](#data-flow)
7. [Best Practices](#best-practices)
8. [Starter Code Guide](#starter-code-guide)

---

## Overview

The Task Tracker application follows a layered architecture that separates:
- **Data Layer** (Model): `Task` and `TodoTracker` classes for data storage and persistence
- **Business Logic** (ViewModel): Mediates between Model and View, handles state and commands
- **Presentation Layer** (View): FTXUI components for terminal UI rendering

This separation enables:
- **Testability**: Business logic can be tested independently of UI
- **Maintainability**: Changes to UI don't affect data logic and vice versa
- **Reusability**: The same Model/ViewModel can power both CLI and TUI interfaces

---

## MVC vs MVVM

### Model-View-Controller (MVC)

```
┌─────────┐       ┌────────────┐       ┌──────────┐
│  Model  │◄──────│ Controller │◄──────│   View   │
│         │       │            │       │          │
│ (Data)  │──────►│  (Logic)   │──────►│   (UI)   │
└─────────┘       └────────────┘       └──────────┘
```

**MVC Characteristics:**
- **Model**: Holds the data (e.g., `Task`, `TodoTracker`)
- **View**: Displays data to the user (FTXUI components)
- **Controller**: Handles user input and updates Model/View

**Pros:**
- Simple and well-understood pattern
- Clear separation of concerns
- Works well for request-response applications (web apps)

**Cons:**
- Controller can become bloated ("fat controller" problem)
- View and Controller are often tightly coupled
- Bi-directional data flow can be complex to debug

### Model-View-ViewModel (MVVM)

```
┌─────────┐       ┌─────────────┐       ┌──────────┐
│  Model  │◄─────►│  ViewModel  │◄═════►│   View   │
│         │       │             │       │          │
│ (Data)  │       │  (State +   │       │   (UI)   │
│         │       │   Commands) │       │          │
└─────────┘       └─────────────┘       └──────────┘
                         ▲
                         │ Data Binding
                         ▼
```

**MVVM Characteristics:**
- **Model**: Pure data structures (e.g., `Task`, `TodoTracker`)
- **View**: UI components that bind to ViewModel state
- **ViewModel**: 
  - Exposes observable state (data the View can bind to)
  - Exposes commands (actions the View can trigger)
  - Contains presentation logic (formatting, validation)

**Pros:**
- Better separation: View knows nothing about Model
- Two-way data binding simplifies state synchronization
- ViewModel is highly testable without UI
- Ideal for reactive/event-driven UIs like TUI applications

**Cons:**
- More boilerplate code (ViewModel layer)
- Data binding can be complex to implement in C++
- Overkill for very simple applications

---

## Recommended Pattern: MVVM

For the Task Tracker TUI application, **MVVM is the recommended pattern** because:

1. **FTXUI is reactive**: Components re-render when state changes, which aligns with MVVM's data binding concept
2. **State management**: The ViewModel holds all UI state (selected task, filter mode, input text), making state predictable
3. **Testability**: ViewModel can be unit tested without FTXUI
4. **Dual interface**: The same ViewModel can potentially power both CLI and TUI

### How MVVM Bridges Data Structures and TUI

```
┌──────────────────────────────────────────────────────────────────┐
│                           MODEL LAYER                            │
├──────────────────────────────────────────────────────────────────┤
│  Task class:              TodoTracker class:                     │
│  - uid                    - std::list<Task> tasks                │
│  - desc                   - add_task()                           │
│  - status                 - delete_task()                        │
│  - creation_date          - mark_task()                          │
│  - updated_date           - serialize()/deserialize()            │
└──────────────────────────────────────────────────────────────────┘
                               │
                               │ ViewModel owns a TodoTracker
                               ▼
┌──────────────────────────────────────────────────────────────────┐
│                        VIEWMODEL LAYER                           │
├──────────────────────────────────────────────────────────────────┤
│  TaskTrackerViewModel:                                           │
│                                                                  │
│  Observable State:                                               │
│  - std::vector<Task> filtered_tasks    (computed from Model)     │
│  - int selected_index                  (UI state)                │
│  - Task::Status current_filter         (UI state)                │
│  - std::string input_text              (UI state)                │
│  - std::string status_message          (UI feedback)             │
│                                                                  │
│  Commands:                                                       │
│  - addTask(desc)          → calls Model, updates filtered_tasks  │
│  - deleteSelectedTask()   → calls Model, updates filtered_tasks  │
│  - markSelectedTask(st)   → calls Model, updates filtered_tasks  │
│  - setFilter(status)      → updates filtered_tasks               │
│  - saveToFile(path)       → persists Model to JSON               │
│  - loadFromFile(path)     → loads Model from JSON                │
│                                                                  │
│  Presentation Logic:                                             │
│  - getTaskDisplayText(t)  → formats task for display             │
│  - getStatusColor(st)     → returns color for status             │
└──────────────────────────────────────────────────────────────────┘
                               │
                               │ View binds to ViewModel state
                               ▼
┌──────────────────────────────────────────────────────────────────┐
│                          VIEW LAYER                              │
├──────────────────────────────────────────────────────────────────┤
│  FTXUI Components:                                               │
│                                                                  │
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │ Task Tracker                                                │ │
│  ├─────────────────────────────────────────────────────────────┤ │
│  │ [Input: New task description...          ]  [Add]           │ │
│  ├─────────────────────────────────────────────────────────────┤ │
│  │ Filter: [All] [Todo] [In Progress] [Done]                   │ │
│  ├─────────────────────────────────────────────────────────────┤ │
│  │  > 1. Buy groceries              [todo]                     │ │
│  │    2. Write documentation        [in-progress]              │ │
│  │    3. Review PR                  [done]                     │ │
│  ├─────────────────────────────────────────────────────────────┤ │
│  │ (a)dd (d)elete (m)ark-done (i)n-progress (q)uit             │ │
│  │ Status: Task added successfully!                            │ │
│  └─────────────────────────────────────────────────────────────┘ │
│                                                                  │
│  Components bind to ViewModel:                                   │
│  - Menu → viewmodel.filtered_tasks, viewmodel.selected_index     │
│  - Input → viewmodel.input_text                                  │
│  - Buttons → viewmodel.addTask(), viewmodel.deleteSelectedTask() │
│  - Text → viewmodel.status_message                               │
└──────────────────────────────────────────────────────────────────┘
```

---

## Architecture Diagram

```
                    ┌─────────────────────────────────────────┐
                    │              main() / App               │
                    │                                         │
                    │  1. Create ViewModel                    │
                    │  2. Create View (pass ViewModel ref)    │
                    │  3. Run ScreenInteractive::Loop()       │
                    └─────────────────────────────────────────┘
                                        │
                     ┌──────────────────┴──────────────────┐
                     ▼                                     ▼
        ┌────────────────────────┐           ┌────────────────────────┐
        │      ViewModel         │           │         View           │
        │                        │◄─────────►│                        │
        │ - TodoTracker model_   │  binding  │ - FTXUI Components     │
        │ - int selected_        │           │ - Renderer             │
        │ - Status filter_       │           │ - Event handlers       │
        │ - string input_        │           │                        │
        │                        │           │                        │
        │ + addTask()            │           │                        │
        │ + deleteTask()         │           │                        │
        │ + getFilteredTasks()   │           │                        │
        └────────────────────────┘           └────────────────────────┘
                     │
                     ▼
        ┌────────────────────────┐
        │        Model           │
        │                        │
        │ - TodoTracker          │
        │   - list<Task>         │
        │   - from_json()        │
        │   - to_json()          │
        │                        │
        │ - Task                 │
        │   - uid, desc, status  │
        │   - dates              │
        └────────────────────────┘
```

---

## Component Breakdown

### 1. Model (`inc/task.h`, `inc/todo.h`)

The Model layer already exists and provides:

```cpp
// Task: Individual task data
class Task {
    uint32_t uid;
    std::string desc;
    Status status;  // ToDo, InProgress, Done
    time_point creation_date;
    time_point updated_date;
};

// TodoTracker: Collection of tasks with persistence
class TodoTracker {
    std::list<Task> tasks;
    
    void from_json(path);   // Load from file
    void to_json(path);     // Save to file
    uint32_t add_task(desc);
    void delete_task(uid);
    void mark_task(uid, status);
};
```

### 2. ViewModel (`inc/task_viewmodel.h`)

The ViewModel bridges Model and View:

```cpp
class TaskTrackerViewModel {
public:
    // Constructor loads tasks from file
    explicit TaskTrackerViewModel(const std::filesystem::path& filepath);
    
    // Observable state (View reads these)
    const std::vector<Task>& getFilteredTasks() const;
    int getSelectedIndex() const;
    Task::Status getCurrentFilter() const;
    const std::string& getInputText() const;
    const std::string& getStatusMessage() const;
    
    // Mutable state for two-way binding
    std::string& inputText();  // For Input component binding
    int& selectedIndex();      // For Menu component binding
    
    // Commands (View calls these)
    void addTask();
    void deleteSelectedTask();
    void markSelectedTask(Task::Status status);
    void setFilter(std::optional<Task::Status> status);  // nullopt = show all
    void save();
    
    // Presentation helpers
    static std::string formatTaskForDisplay(const Task& task);
    static ftxui::Color getStatusColor(Task::Status status);

private:
    TodoTracker model_;
    std::filesystem::path filepath_;
    
    // UI State
    std::vector<Task> filtered_tasks_;
    int selected_index_ = 0;
    std::optional<Task::Status> current_filter_;
    std::string input_text_;
    std::string status_message_;
    
    void refreshFilteredTasks();
};
```

### 3. View (`app/tui.cpp`)

The View uses FTXUI components bound to ViewModel state:

```cpp
int main() {
    // Initialize ViewModel
    TaskTrackerViewModel viewmodel(get_config_path());
    
    // Create FTXUI components bound to ViewModel
    auto input = Input(&viewmodel.inputText(), "New task...");
    
    auto task_menu = Menu(&task_entries, &viewmodel.selectedIndex());
    
    // Renderer combines components and reads ViewModel state
    auto renderer = Renderer(container, [&] {
        // Build UI from ViewModel state
        return window(text("Task Tracker"), vbox({
            hbox({input->Render(), button_add->Render()}),
            separator(),
            task_menu->Render(),
            separator(),
            text(viewmodel.getStatusMessage()),
        }));
    });
    
    // Event handler calls ViewModel commands
    renderer |= CatchEvent([&](Event e) {
        if (e == Event::Character('a')) {
            viewmodel.addTask();
            return true;
        }
        // ... other keybindings
    });
    
    auto screen = ScreenInteractive::Fullscreen();
    screen.Loop(renderer);
}
```

---

## Data Flow

### Adding a Task (User Action → Model Update → View Refresh)

```
1. User types "Buy milk" and presses Enter
         │
         ▼
2. FTXUI Input component updates viewmodel.input_text_
         │
         ▼
3. CatchEvent handler detects Enter key
         │
         ▼
4. Handler calls viewmodel.addTask()
         │
         ▼
5. ViewModel::addTask() {
     a. model_.add_task(input_text_);   // Update Model
     b. input_text_.clear();             // Clear input
     c. refreshFilteredTasks();          // Recompute filtered list
     d. status_message_ = "Task added!"; // Update feedback
   }
         │
         ▼
6. FTXUI re-renders (automatic - state changed)
         │
         ▼
7. Renderer lambda reads ViewModel state:
     - filtered_tasks_ now includes new task
     - input_text_ is empty
     - status_message_ shows success
         │
         ▼
8. User sees updated task list
```

### Filtering Tasks (No Model Change, Just View State)

```
1. User presses 'd' to filter by "Done"
         │
         ▼
2. CatchEvent handler calls viewmodel.setFilter(Status::Done)
         │
         ▼
3. ViewModel::setFilter(Status::Done) {
     a. current_filter_ = Status::Done;  // Update filter state
     b. refreshFilteredTasks();           // Recompute filtered list
     c. selected_index_ = 0;              // Reset selection
   }
         │
         ▼
4. ViewModel::refreshFilteredTasks() {
     filtered_tasks_.clear();
     for (const auto& task : model_.getTasks()) {
       if (!current_filter_ || task.getStatus() == *current_filter_) {
         filtered_tasks_.push_back(task);
       }
     }
   }
         │
         ▼
5. FTXUI re-renders with new filtered_tasks_
```

---

## Best Practices

### 1. Keep the Model Pure

The Model (`Task`, `TodoTracker`) should:
- ✅ Contain only data and persistence logic
- ✅ Be independent of any UI framework
- ❌ NOT contain FTXUI types or UI state

```cpp
// Good: Model is pure data
class Task {
    uint32_t uid;
    std::string desc;
    Status status;
};

// Bad: Model contains UI concerns
class Task {
    uint32_t uid;
    std::string desc;
    ftxui::Color display_color;  // ❌ UI concern!
};
```

### 2. Put Presentation Logic in ViewModel

```cpp
// Good: ViewModel handles display formatting
class TaskTrackerViewModel {
    static std::string formatTaskForDisplay(const Task& task) {
        return std::format("{}. {} [{}]", 
            task.getUid(), 
            task.getDesc(), 
            Task::Stat2String.at(task.getStatus()));
    }
    
    static ftxui::Color getStatusColor(Task::Status status) {
        switch (status) {
            case Task::Status::ToDo: return ftxui::Color::Yellow;
            case Task::Status::InProgress: return ftxui::Color::Blue;
            case Task::Status::Done: return ftxui::Color::Green;
        }
    }
};
```

### 3. Use References for Two-Way Binding

FTXUI components like `Input` and `Menu` require pointer/reference to state:

```cpp
// ViewModel exposes mutable references for binding
class TaskTrackerViewModel {
    std::string& inputText() { return input_text_; }
    int& selectedIndex() { return selected_index_; }
};

// View binds components to these references
auto input = Input(&viewmodel.inputText(), "placeholder");
auto menu = Menu(&entries, &viewmodel.selectedIndex());
```

### 4. Centralize Event Handling

Use a single `CatchEvent` handler for keyboard shortcuts:

```cpp
auto renderer = Renderer(...) | CatchEvent([&](Event event) {
    if (event == Event::Character('q')) {
        screen.ExitLoopClosure()();
        return true;
    }
    if (event == Event::Character('a')) {
        viewmodel.addTask();
        return true;
    }
    if (event == Event::Character('d')) {
        viewmodel.deleteSelectedTask();
        return true;
    }
    // ... more keybindings
    return false;  // Event not handled
});
```

### 5. Save on Exit or After Changes

```cpp
// Option A: Auto-save after each change
void TaskTrackerViewModel::addTask() {
    model_.add_task(input_text_);
    save();  // Persist immediately
    refreshFilteredTasks();
}

// Option B: Save on exit only
screen.Loop(renderer);
viewmodel.save();  // Save when loop exits
```

### 6. Handle Edge Cases

```cpp
void TaskTrackerViewModel::deleteSelectedTask() {
    const auto& tasks = getFilteredTasks();
    if (tasks.empty()) {
        status_message_ = "No tasks to delete";
        return;
    }
    if (selected_index_ < 0 || selected_index_ >= tasks.size()) {
        status_message_ = "Invalid selection";
        return;
    }
    
    auto uid = tasks[selected_index_].getUid();
    model_.delete_task(uid);
    refreshFilteredTasks();
    
    // Adjust selection if we deleted the last item
    if (selected_index_ >= filtered_tasks_.size()) {
        selected_index_ = std::max(0, (int)filtered_tasks_.size() - 1);
    }
    
    status_message_ = "Task deleted";
}
```

---

## Starter Code Guide

See `inc/task_viewmodel.h` for the ViewModel implementation and `app/tui.cpp` for the View implementation.

### File Structure

```
task-tracker/
├── inc/
│   ├── task.h              # Task model (existing)
│   ├── todo.h              # TodoTracker model (existing)
│   └── task_viewmodel.h    # NEW: ViewModel for TUI
├── src/
│   ├── task.cpp            # Task implementation (existing)
│   ├── todo.cpp            # TodoTracker implementation (existing)
│   └── task_viewmodel.cpp  # NEW: ViewModel implementation
├── app/
│   ├── cli.cpp             # CLI application (existing)
│   └── tui.cpp             # TUI application (updated with MVVM)
└── docs/
    ├── architecture.md     # This document
    └── ftxui.md            # FTXUI usage notes
```

### Building and Running

```bash
# Build
make build

# Run TUI
./build/task-tui

# Run CLI (still works)
./build/task-cli help
```

### Key Bindings (Suggested)

| Key | Action |
|-----|--------|
| `Enter` | Add task (when input focused) |
| `↑/↓` | Navigate task list |
| `d` | Delete selected task |
| `m` | Mark selected as done |
| `i` | Mark selected as in-progress |
| `t` | Mark selected as todo |
| `1-4` | Filter: All/Todo/InProgress/Done |
| `q` | Quit and save |

---

## Summary

The MVVM pattern provides a clean architecture for the Task Tracker TUI:

1. **Model** (`Task`, `TodoTracker`): Pure data and persistence
2. **ViewModel** (`TaskTrackerViewModel`): UI state, commands, and presentation logic
3. **View** (FTXUI in `tui.cpp`): Components bound to ViewModel state

This separation makes the code:
- **Testable**: ViewModel can be unit tested without UI
- **Maintainable**: Clear boundaries between layers
- **Extensible**: Easy to add features without breaking existing code
