#include "ftxui/component/component.hpp"      // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp" // for ComponentBase
#include "ftxui/component/screen_interactive.hpp" // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp" // for text, hbox, separator, Element, operator|, vbox, border
#include <ftxui/dom/deprecated.hpp>
#include <ftxui/dom/node.hpp>
#include <ranges>
#include <vector>

using namespace ftxui;

ComponentDecorator handler_quit(ScreenInteractive &screen) {
  return CatchEvent([&](Event event) {
    if (event == Event::q or event == Event::Q) {
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });
}

int main(int argc, char *argv[]) {
  std::vector<std::string> tasks;
  std::string cur_task;
  std::vector<Element> ts;

  Component input_task = Input(&cur_task, "Task:");
  input_task |= CatchEvent([&tasks, &cur_task](Event event) {
    if (event == Event::Return) {
      tasks.push_back(cur_task);
      cur_task.clear();
      return true;
    }
    return false;
  });

  auto screen = ScreenInteractive::Fullscreen();
  auto component = Container::Vertical({input_task});
  auto renderer = Renderer(component, [&] {
    ts = std::vector<Element>(tasks.size());
    std::transform(tasks.begin(), tasks.end(), ts.begin(),
                   [](const auto &t) { return text(t); });

    auto display_ele = vbox({
        text("Enter a new task"),
        input_task->Render(),
        vbox(ts),
        hbox({
            text("left") | border,
            text("middle") | border,
            text("right") | border,
        }),
        separator(),
        text("Press (q) to quit"),
    });

    return vbox({window(text("Task Tracker"), display_ele)});
  });
  // renderer | handler_quit(screen);
  screen.Loop(renderer);
}
