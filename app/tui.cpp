#include <ftxui/dom/node.hpp>
#include <string> // for char_traits, operator+, string, basic_string
#include <vector>

#include "ftxui/component/component.hpp"      // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp" // for ComponentBase
#include "ftxui/component/component_options.hpp" // for InputOption
#include "ftxui/component/screen_interactive.hpp" // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp" // for text, hbox, separator, Element, operator|, vbox, border
#include "ftxui/util/ref.hpp" // for Ref

int main(int argc, char *argv[]) {
  using namespace ftxui;

  // The data:
  std::string task;
  std::vector<std::string> tasks;

  // The basic input components:
  Component input_task = Input(&task, "write a short description");

  input_task |= CatchEvent(
      [&](Event event) { return event.is_character() && task.size() > 80; });

  // The component tree:
  auto component = Container::Vertical({input_task});

  // Tweak how the component tree is rendered:
  auto renderer = Renderer(component, [&] {
    return vbox({
               window(text("tasks"),
                      vbox({
                          hbox(text("input: "), input_task->Render()),
                          separator(),
                          text("task: " + task),
                      })),
           }) |
           border;
  });

  auto screen = ScreenInteractive::Fullscreen();
  screen.Loop(renderer);
}
