// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <filesystem>
#include <format>
#include <ftxui/component/component_options.hpp> // for ButtonOption
#include <ftxui/component/mouse.hpp>             // for ftxui
#include <functional>                            // for function

#include "todo_viewmodel.h"

#include "ftxui/component/component.hpp" // for Button, operator|=, Renderer, Vertical, Modal
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive, Component
#include "ftxui/dom/elements.hpp" // for operator|, separator, text, size, Element, vbox, border, GREATER_THAN, WIDTH, center, HEIGHT

using namespace ftxui;

auto button_style = ButtonOption::Simple();

Component MainComponent(TodoViewModel &tvm,
                        std::function<void()> addtask_show_modal,
                        std::function<void()> exit) {
  // define the menulist of task items
  // the entries are defined via const reference, and the index is a mutable ref
  auto tasklist = Menu(&tvm.get_task_entries_const(), &tvm.get_seleted_task());

  auto component = Container::Vertical({
      Button("Add Task", addtask_show_modal, button_style),
      tasklist,
      Button("Quit", exit, button_style),
  });

  // Polish how the two buttons are rendered:
  component |= Renderer([&](Element inner) {
    auto idx = tvm.get_selected_task_const();
    auto comp = vbox({
        inner,
        separator(),
        text(std::format("Current selected text: {}",
                         tvm.get_task_entries().at(idx))),
        text(std::format("Current selected input: {}", idx)),
    });

    return window(text("Task Tracker") | bold | center, comp);
  });

  return component;
}

Component AddTaskComponent(TodoViewModel &tvm,
                           std::function<void()> hide_modal) {

  auto input_options = InputOption::Spacious();
  input_options.multiline = false;
  input_options.on_enter = [&tvm] { tvm.add_task(); };

  auto text_field =
      Input(&tvm.get_input_text(), "Enter a task!", input_options);

  auto component = Container::Vertical({
      text_field,
      Button("Quit", hide_modal, button_style),
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

  component |= CatchEvent([&hide_modal](Event event) {
    if (event == Event::Escape) {
      hide_modal();
      return true;
    }
    return false;
  });

  return component;
}

int main(int argc, const char *argv[]) {
  auto screen = ScreenInteractive::Fullscreen();

  // TODO: add this shared routine to a util file
  std::string filename = "todo.json";
  std::filesystem::path filepath = filename;
  // create the todo list under home if possible
  if (auto home_dir = std::getenv("HOME"); home_dir != nullptr) {
    filepath = filepath / home_dir / filename;
  }
  TodoViewModel tvm(filepath);

  // State of the application:
  auto addtask_input_shown = false;

  // Some actions modifying the state:
  auto addtask_show_modal = [&] { addtask_input_shown = true; };
  auto addtask_hide_modal = [&] { addtask_input_shown = false; };
  auto exit_prog = screen.ExitLoopClosure();

  // Instanciate the main and modal components:
  auto main_component = MainComponent(tvm, addtask_show_modal, exit_prog);
  auto add_task_component = AddTaskComponent(tvm, addtask_hide_modal);

  main_component |=
      CatchEvent([&tvm, &addtask_show_modal, &exit_prog](Event event) {
        if (event == Event::d) {
          tvm.delete_selected_task();
          return true;
        }
        if (event == Event::n) {
          addtask_show_modal();
          return true;
        }
        if (event == Event::q) {
          exit_prog();
          return true;
        }
        return false;
      });

  // Use the `Modal` function to use together the main component and its modal
  // window. The |modal_shown| boolean controls whether the modal is shown or
  // not.
  main_component |= Modal(add_task_component, &addtask_input_shown);

  screen.Loop(main_component);
  return 0;
}
