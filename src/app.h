#ifndef TETRIS3D_APP_H_
#define TETRIS3D_APP_H_

#include <chrono>
#include <cstdlib>
#include <memory>
#include <stdio.h>
#include <thread>

#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

#include "config.h"
#include "game/game.h"
#include "input.h"
#include "timer.h"

namespace tetris3d {

class App {
public:
    App(const Config& config);

    void Run();
    void OnKeyChanged(int key, int scancode, int action, int mods);
    void OnFramebufferResize(int width, int height);

private:
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    bool StartUp();

    // NOTE(panmar): config_ should be the first member due to initialization
    // order
    Config config_;
    std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> window_{nullptr,
                                                               nullptr};
    Timer timer_;
    Input input_;
    Game game_;
};

} // namespace tetris3d

#endif // TETRIS3D_APP_H_
