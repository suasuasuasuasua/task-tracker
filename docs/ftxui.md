# FTXUI Notes

A comprehensive guide to using FTXUI for the Task Tracker TUI application.

## Links

- [Live docs](https://arthursonzogni.github.io/FTXUI/)
- [GitHub link](https://github.com/ArthurSonzogni/FTXUI)
- [Examples](https://github.com/ArthurSonzogni/FTXUI/tree/main/examples)

---

## Core Concepts

FTXUI has three main modules:

### 1. `ftxui::dom` - Document Object Model

Defines the structure of UI elements (like HTML for terminals):

```cpp
#include "ftxui/dom/elements.hpp"

// Basic elements
Element e = text("Hello");
Element e = paragraph("Long text that wraps...");
Element e = separator();

// Containers
Element e = vbox({text("Line 1"), text("Line 2")});  // Vertical
Element e = hbox({text("Left"), text("Right")});     // Horizontal
Element e = dbox({background, foreground});          // Stacked

// Decorators (modifiers)
Element e = text("Styled") | bold | color(Color::Red);
Element e = text("Boxed") | border;
Element e = text("Centered") | center;
Element e = text("Sized") | size(WIDTH, EQUAL, 20);
```

### 2. `ftxui::screen` - Rendering

Renders Elements to the terminal:

```cpp
#include "ftxui/screen/screen.hpp"

// Static rendering (for one-shot output)
Element document = vbox({text("Hello"), text("World")});
auto screen = Screen::Create(Dimension::Fit(document));
Render(screen, document);
screen.Print();
```

### 3. `ftxui::component` - Interactive Components

Components are stateful, interactive elements:

```cpp
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"

// Built-in components
std::string name;
Component input = Input(&name, "Enter name...");

int selected = 0;
std::vector<std::string> entries = {"Option 1", "Option 2"};
Component menu = Menu(&entries, &selected);

bool checked = false;
Component checkbox = Checkbox("Enable feature", &checked);

Component button = Button("Click me", [] { /* action */ });
```

---

## Component Patterns

### Creating Interactive Applications

```cpp
int main() {
    // 1. Create a screen (manages terminal state)
    auto screen = ScreenInteractive::Fullscreen();
    // Alternatives:
    // auto screen = ScreenInteractive::TerminalOutput();  // Just terminal size
    // auto screen = ScreenInteractive::FitComponent();    // Fits content

    // 2. Create components with bound state
    std::string input_text;
    auto input = Input(&input_text, "placeholder");
    
    // 3. Create a renderer that builds the UI
    auto renderer = Renderer(input, [&] {
        return vbox({
            text("Enter text:"),
            input->Render(),
            text("You typed: " + input_text),
        }) | border;
    });
    
    // 4. Run the event loop
    screen.Loop(renderer);
}
```

### Combining Multiple Components

```cpp
// Vertical container (navigate with ↑/↓)
auto container = Container::Vertical({
    input,
    menu,
    button,
});

// Horizontal container (navigate with ←/→)
auto container = Container::Horizontal({
    menu1,
    menu2,
});

// Tab container (switch with Tab key)
int tab_selected = 0;
auto container = Container::Tab({
    tab1_content,
    tab2_content,
}, &tab_selected);
```

### Custom Event Handling

```cpp
// CatchEvent intercepts events before component handles them
auto component = Container::Vertical({input, menu});
component |= CatchEvent([&](Event event) {
    // Handle specific keys
    if (event == Event::Character('q')) {
        screen.ExitLoopClosure()();
        return true;  // Event consumed
    }
    if (event == Event::Return) {
        // Handle Enter key
        return true;
    }
    return false;  // Event not handled, pass to component
});

// Common events:
// Event::Character('x')  - Single character
// Event::Return          - Enter key
// Event::Escape          - Escape key
// Event::Tab             - Tab key
// Event::ArrowUp/Down/Left/Right
// Event::Backspace
// Event::Delete
```

### Renderer Pattern (Separating Logic from Layout)

```cpp
auto renderer = Renderer(component, [&] {
    // This lambda is called on every render
    // It should build the UI from current state
    
    Elements task_elements;
    for (const auto& task : viewmodel.getFilteredTasks()) {
        task_elements.push_back(text(format_task(task)));
    }
    
    return window(text("Task Tracker"), vbox({
        hbox({text("Filter: "), filter_menu->Render()}),
        separator(),
        vbox(task_elements) | frame | flex,
        separator(),
        text(viewmodel.getStatusMessage()) | dim,
    }));
});
```

---

## Styling and Layout

### Colors

```cpp
// Named colors
text("Red") | color(Color::Red);
text("Green") | bgcolor(Color::Green);

// 256 colors
text("Orange") | color(Color::RGB(255, 165, 0));

// Palette colors (0-255)
text("Color") | color(Color::Palette256(202));
```

### Borders and Windows

```cpp
// Simple border
element | border;

// Named window (border with title)
window(text("Title"), content);

// Border styles
element | borderLight;
element | borderHeavy;
element | borderDouble;
element | borderRounded;
```

### Size and Flex

```cpp
// Fixed size
element | size(WIDTH, EQUAL, 20);
element | size(HEIGHT, EQUAL, 5);

// Flex (fill available space)
element | flex;
element | flex_grow;
element | flex_shrink;

// Size constraints
element | size(WIDTH, LESS_THAN, 50);
element | size(HEIGHT, GREATER_THAN, 10);
```

### Alignment

```cpp
element | center;
element | vcenter;
element | hcenter;
element | align_right;
```

---

## Menu Component (Task List)

For the task list, `Menu` is ideal:

```cpp
int selected = 0;
std::vector<std::string> entries;

// Update entries from model
void refreshEntries() {
    entries.clear();
    for (const auto& task : model.getTasks()) {
        entries.push_back(format_task(task));
    }
}

// Create menu
auto menu = Menu(&entries, &selected);

// With custom styling
MenuOption option;
option.on_enter = [&] { /* action when Enter pressed */ };
option.focused_entry = &selected;
option.entries_option.transform = [](EntryState state) {
    Element e = text(state.label);
    if (state.focused) {
        e = e | bold | color(Color::Cyan);
    }
    if (state.active) {
        e = e | bgcolor(Color::Blue);
    }
    return e;
};
auto menu = Menu(&entries, &selected, option);
```

---

## Input Component (Add Task)

```cpp
std::string input_text;

// Basic input
auto input = Input(&input_text, "New task description...");

// With options
InputOption option;
option.on_enter = [&] {
    if (!input_text.empty()) {
        model.add_task(input_text);
        input_text.clear();
    }
};
option.multiline = false;
option.password = false;
auto input = Input(&input_text, "New task...", option);
```

---

## Radiobox/Toggle (Filter Selection)

```cpp
int filter_selected = 0;  // 0=All, 1=Todo, 2=InProgress, 3=Done
std::vector<std::string> filter_options = {"All", "Todo", "In Progress", "Done"};

// Horizontal toggle (using Radiobox)
auto filter = Radiobox(&filter_options, &filter_selected);

// Or using Toggle
auto filter = Toggle(&filter_options, &filter_selected);

// In renderer, apply filter based on selection
auto renderer = Renderer(container, [&] {
    std::optional<Task::Status> filter;
    switch (filter_selected) {
        case 1: filter = Task::Status::ToDo; break;
        case 2: filter = Task::Status::InProgress; break;
        case 3: filter = Task::Status::Done; break;
    }
    // Use filter...
});
```

---

## Complete Example: Task Tracker Layout

```cpp
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

using namespace ftxui;

int main() {
    // State
    std::string input_text;
    int selected_task = 0;
    int selected_filter = 0;
    std::vector<std::string> task_entries = {"1. Buy groceries [todo]"};
    std::vector<std::string> filter_options = {"All", "Todo", "In Progress", "Done"};
    std::string status = "Ready";

    // Components
    auto input = Input(&input_text, "New task...");
    auto task_menu = Menu(&task_entries, &selected_task);
    auto filter = Toggle(&filter_options, &selected_filter);
    auto add_btn = Button("Add", [&] {
        if (!input_text.empty()) {
            task_entries.push_back(std::to_string(task_entries.size() + 1) + 
                                   ". " + input_text + " [todo]");
            input_text.clear();
            status = "Task added!";
        }
    });
    
    // Layout
    auto container = Container::Vertical({
        Container::Horizontal({input, add_btn}),
        filter,
        task_menu,
    });
    
    auto renderer = Renderer(container, [&] {
        return window(text("Task Tracker"), vbox({
            hbox({
                text("New: "),
                input->Render() | flex,
                add_btn->Render(),
            }),
            separator(),
            hbox({text("Filter: "), filter->Render()}),
            separator(),
            task_menu->Render() | frame | flex,
            separator(),
            hbox({
                text("(a)dd "),
                text("(d)elete ") | color(Color::Red),
                text("(m)ark done ") | color(Color::Green),
                text("(q)uit"),
            }) | dim,
            text(status) | color(Color::Yellow),
        }));
    });
    
    // Event handling
    auto screen = ScreenInteractive::Fullscreen();
    renderer |= CatchEvent([&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::Character('d') && !task_entries.empty()) {
            task_entries.erase(task_entries.begin() + selected_task);
            selected_task = std::min(selected_task, (int)task_entries.size() - 1);
            status = "Task deleted";
            return true;
        }
        return false;
    });
    
    screen.Loop(renderer);
    return 0;
}
```

---

## Tips for Task Tracker

1. **Use `frame` for scrollable lists**: `task_menu->Render() | frame | flex`
2. **Use `flex` to fill space**: Important for fullscreen layouts
3. **Use `dim` for secondary text**: Help text, status messages
4. **Store component state in ViewModel**: Keep UI state out of View layer
5. **Call `screen.PostEvent(Event::Custom)` to trigger rerender**: After async updates
6. **Use `Maybe()` for conditional rendering**: Hide components when not needed

```cpp
bool show_help = false;
auto help_panel = Container::Vertical({...});
auto maybe_help = Maybe(help_panel, &show_help);
```

---

## Debugging

```cpp
// Print to stderr (won't interfere with TUI)
std::cerr << "Debug: selected = " << selected << std::endl;

// Or use a status line in your UI
std::string debug_info;
// In renderer:
text(debug_info) | dim
```
