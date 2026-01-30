// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <filesystem>
#include <ftxui/component/component_options.hpp> // for ButtonOption
#include <ftxui/component/mouse.hpp>             // for ftxui
#include <functional>                            // for function
#include <sstream>

#include "todo_viewmodel.h"

#include "ftxui/component/component.hpp" // for Button, operator|=, Renderer, Vertical, Modal
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive, Component
#include "ftxui/dom/elements.hpp" // for operator|, separator, text, size, Element, vbox, border, GREATER_THAN, WIDTH, center, HEIGHT

using namespace ftxui;

auto button_style = ButtonOption::Simple();

// Definition of the main component. The details are not important.
Component MainComponent(TodoViewModel &tvm, std::function<void()> show_modal,
                        std::function<void()> exit) {
  auto component = Container::Vertical({
      Button("Add Task", show_modal, button_style),
      Button("Quit", exit, button_style),
  });
  // Polish how the two buttons are rendered:
  component |= Renderer([&](Element inner) {
    auto tasks = tvm.get_tasks();
    Elements task_elements;
    std::stringstream ss;
    for (const auto &t : tasks) {
      ss.str(std::string());
      ss << t;
      task_elements.emplace_back(text(ss.str()));
    }

    auto comp = vbox({
        text("Main component"),
        separator(),
        inner,
        separator(),
        hbox({
            text("Current text: "),
            text(tvm.get_input_text_const()),
        }),
        separator(),
        vbox(task_elements),
    });

    return window(text("Task Tracker"), comp);
  });
  return component;
}

// Definition of the modal component. The details are not important.
Component ModalComponent(std::string &text_data,
                         std::function<void()> do_nothing,
                         std::function<void()> hide_modal) {

  auto input_options = InputOption::Spacious();
  input_options.multiline = false;

  auto text_field = Input(&text_data, "Enter a task!", input_options);

  auto component = Container::Vertical({
      text_field,
      Button("Do nothing", do_nothing, button_style),
      Button("Quit modal", hide_modal, button_style),
  });
  // Polish how the two buttons are rendered:
  component |= Renderer([&](Element inner) {
    return vbox({
               text("Modal component "),
               separator(),
               inner,
           })                              //.
           | size(WIDTH, GREATER_THAN, 30) //
           | border;                       //
  });

  return component;
}

int main(int argc, const char *argv[]) {
  auto screen = ScreenInteractive::Fullscreen();

  // TODO: add this routine to a util file
  std::string filename = "todo.json";
  std::filesystem::path filepath = filename;
  // create the todo list under home if possible
  if (auto home_dir = std::getenv("HOME"); home_dir != nullptr) {
    filepath = filepath / home_dir / filename;
  }
  TodoViewModel tvm(filepath);

  // State of the application:
  bool task_input_shown = false;

  // Some actions modifying the state:
  auto show_modal = [&] { task_input_shown = true; };
  auto hide_modal = [&] { task_input_shown = false; };
  auto exit = screen.ExitLoopClosure();
  auto do_nothing = [&] {};

  // Instanciate the main and modal components:
  auto main_component = MainComponent(tvm, show_modal, exit);
  auto modal_component =
      ModalComponent(tvm.get_input_text(), do_nothing, hide_modal);

  // Use the `Modal` function to use together the main component and its modal
  // window. The |modal_shown| boolean controls whether the modal is shown or
  // not.
  main_component |= Modal(modal_component, &task_input_shown);

  screen.Loop(main_component);
  return 0;
}
