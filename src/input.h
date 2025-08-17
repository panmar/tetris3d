#ifndef TETRIS3D_INPUT_H_
#define TETRIS3D_INPUT_H_

#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

#include <array>

namespace tetris3d {

class Input {
public:
    Input();

    void Update();
    void OnKeyChanged(int key, int scancode, int action, int mods);
    bool IsKeyDown(int key) const;
    bool IsKeyPressed(int key) const;

private:
    std::array<int, GLFW_KEY_LAST + 1> keyboard_;
    std::array<int, GLFW_KEY_LAST + 1> prev_keyboard_;
};

} // namespace tetris3d

#endif // TETRIS3D_INPUT_H_
