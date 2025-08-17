#include "input.h"

#include <cassert>

namespace tetris3d {

Input::Input() {
    prev_keyboard_.fill(GLFW_RELEASE);
    keyboard_.fill(GLFW_RELEASE);
}

void Input::Update() { prev_keyboard_ = keyboard_; }

void Input::OnKeyChanged(int key, int scancode, int action, int mods) {
    // NOTE(panmar): GLFW_REPEAT is converted to GLFW_PRESS
    keyboard_[key] = (action == GLFW_RELEASE ? GLFW_RELEASE : GLFW_PRESS);
}

bool Input::IsKeyDown(int key) const {
    assert(key < static_cast<int>(keyboard_.size()));
    return keyboard_[key] == GLFW_PRESS;
}

bool Input::IsKeyPressed(int key) const {
    assert(key < static_cast<int>(keyboard_.size()));
    return keyboard_[key] == GLFW_PRESS && prev_keyboard_[key] == GLFW_RELEASE;
}

} // namespace tetris3d