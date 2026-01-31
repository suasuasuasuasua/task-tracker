# Architecture

This document describes the architecture of the Task Tracker application.

## Overview

The Task Tracker is a C++20 application that provides both CLI and TUI interfaces for managing tasks. It follows a Model-View-ViewModel (MVVM) architecture pattern for the TUI and a simpler Model pattern for the CLI.

## Components

### Core Model (`src/`)

- **Task** (`task.h/cpp`): Represents a single task with properties:
  - `uid`: Unique identifier (uint32_t)
  - `desc`: Task description
  - `status`: Current status (ToDo, InProgress, Done)
  - `creation_date`: When the task was created
  - `updated_date`: When the task was last modified
  
  Provides serialization/deserialization to/from JSON format.

- **TodoTracker** (`todo.h/cpp`): Manages a collection of tasks
  - Maintains tasks in a `std::map` indexed by UID
  - Provides CRUD operations: add, update, delete, mark status
  - Handles JSON file I/O with error handling
  - Automatically generates unique IDs for new tasks

### View Model (`src/`)

- **TodoViewModel** (`todo_viewmodel.h/cpp`): TUI-specific layer
  - Mediates between the Model and FTXUI components
  - Manages UI state (selected task, input visibility, filtered tasks)
  - Provides UI callbacks for adding, deleting, and updating tasks
  - Automatically saves changes to disk on destruction

### Applications (`app/`)

- **task-cli** (`cli.cpp`): Command-line interface
  - Simple command-based interaction
  - Supports: add, update, delete, mark, list operations
  - Input validation with error handling

- **task-tui** (`tui.cpp`): Terminal UI using FTXUI
  - Interactive fullscreen interface
  - Keyboard shortcuts for all operations
  - Modal dialog for adding tasks
  - Logging via spdlog

## Data Flow

### CLI Flow
```
User Input → CLI Parser → TodoTracker → JSON File
```

### TUI Flow
```
User Input → FTXUI Events → TodoViewModel → TodoTracker → JSON File
```

## File Storage

- Tasks are stored in `$HOME/todo.json` (or current directory as fallback)
- Logs are stored in `$HOME/.local/state/task-tracker/log.txt`
- JSON format with metadata (title, task count, task array)

## Error Handling

- File I/O operations wrapped in try-catch blocks
- JSON parsing errors caught and logged
- Input validation for IDs and descriptions
- Graceful fallbacks (e.g., HOME directory not set)

## Build System

- CMake-based build system
- Dependencies fetched via FetchContent (FTXUI, nlohmann_json, spdlog, GTest)
- Support for both Nix and traditional builds
- Unit tests using Google Test

## Code Quality

- Modern C++20 features (ranges, concepts, chrono)
- `#pragma once` for header guards
- Consistent error handling patterns
- Comprehensive unit test coverage for core functionality
