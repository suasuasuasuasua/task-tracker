# FTXUI notes

- A container is an organized list of components
- Components are Elements that contain state
  - for example, buttons, menus, toggles, etc.
  - these items need to have additional variables like boolean to facilitate the
    state logic
- Renderers can be defined on components
  - If you pass it with `|=`, then the component is the `inner` parameter in the
    lambda function
  - The render function is actually what drawn to the screen
  - You can't define Components inside of the Render block unless you call
    `->Render()` on the object

## Examples

- [Modal
  popup](https://github.com/ArthurSonzogni/FTXUI/blob/main/examples/component/modal_dialog.cpp)
  - It may be useful to have a press "n" to create a new note

## Links

- [Live docs](https://arthursonzogni.github.io/FTXUI/)
- [Github link](https://github.com/ArthurSonzogni/FTXUI)
