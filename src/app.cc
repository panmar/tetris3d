#include "app.h"

namespace {

GLFWwindow* CreateWindow(const tetris3d::Config& config) {
    if (config.graphics_renderer_type ==
        tetris3d::Config::RendererType::Basic) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    } else {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#if __APPLE__
        // TODO(panmar): Check if this option works on Linux as well
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    glfwWindowHint(GLFW_SAMPLES, config.graphics_multisampling_samples);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (config.graphics_fullscreen && config.graphics_borderless) {
        const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, video_mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, video_mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, video_mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, video_mode->refreshRate);
        return glfwCreateWindow(video_mode->width, video_mode->height,
                                config.window_title.c_str(), monitor, nullptr);
    } else if (config.graphics_fullscreen) {
        return glfwCreateWindow(config.graphics_resolution_width,
                                config.graphics_resolution_height,
                                config.window_title.c_str(), monitor, nullptr);
    } else {
        return glfwCreateWindow(config.graphics_resolution_width,
                                config.graphics_resolution_height,
                                config.window_title.c_str(), monitor, nullptr);
    }
}

void OnFramebufferResize(GLFWwindow* window, int width, int height) {
    auto app =
        reinterpret_cast<tetris3d::App*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->OnFramebufferResize(width, height);
    }
}

void OnKeyCallback(GLFWwindow* window, int key, int scancode, int action,
                   int mods) {
    auto app =
        reinterpret_cast<tetris3d::App*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->OnKeyChanged(key, scancode, action, mods);
    }
}

} // namespace

namespace tetris3d {

App::App(const Config& config) : config_(config), game_(config_) {}

void App::Run() {
    using namespace std::literals::chrono_literals;

    if (!StartUp()) {
        return;
    }
    if (!game_.StartUp()) {
        return;
    }

    // NOTE(panmar): Even if we set glfwSetFramebufferSizeCallback callback
    // during Tetris3DApp::StartUp(), it is not triggered
    // when framebuffer is initialized, so, at the beginning, we need to
    // trigger it manually
    {
        int framebuffer_width, framebuffer_height;
        glfwGetFramebufferSize(window_.get(), &framebuffer_width,
                               &framebuffer_height);
        game_.OnFramebufferResize(framebuffer_width, framebuffer_height);
    }

    timer_.Update();
    while (!glfwWindowShouldClose(window_.get())) {
        input_.Update();
        glfwPollEvents();
        timer_.Update();
        game_.Update(input_, timer_.GetElapsedSeconds());

        if (input_.IsKeyPressed(config_.key_quit)) {
            glfwSetWindowShouldClose(window_.get(), GLFW_TRUE);
        }

        game_.Draw();
        glfwSwapBuffers(window_.get());

        std::this_thread::sleep_for(16ms);
    }
}

void App::OnKeyChanged(int key, int scancode, int action, int mods) {
    input_.OnKeyChanged(key, scancode, action, mods);
}

void App::OnFramebufferResize(int width, int height) {
    game_.OnFramebufferResize(width, height);
}

bool App::StartUp() {
    srand(config_.rand_seed);
    glfwInit();

    window_ = std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)>(
        CreateWindow(config_), [](GLFWwindow* w) { glfwDestroyWindow(w); });

    if (!window_) {
        glfwTerminate();
        throw Error("GLFW window creation failure");
    }

    glfwMakeContextCurrent(window_.get());

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw Error("GLAD initialization failuire");
    }

    // NOTE(panmar): GLFW library uses C-style global callbacks mechanism;
    // We pass App ptr here, so we can access it during global callbacks
    glfwSetWindowUserPointer(window_.get(), this);
    glfwSetFramebufferSizeCallback(window_.get(), ::OnFramebufferResize);
    glfwSetKeyCallback(window_.get(), OnKeyCallback);

    return true;
}

} // namespace tetris3d
