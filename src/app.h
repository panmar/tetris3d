#pragma once

#include <chrono>
#include <cstdlib>
#include <stdio.h>
#include <thread>

#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"
#include "game.h"
#include "input.h"
#include "settings.h"

void OnFramebufferResize(GLFWwindow* window, i32 width, i32 height);
void OnKeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action,
                   i32 mods);

class Timer {
  public:
    void Tick() {
        auto tick = std::chrono::high_resolution_clock::now();
        std::chrono::duration<f32> diff = tick - prev_tick;
        elapsed_seconds = diff.count();
        prev_tick = tick;
    }

    // Return elapsed seconds between ticks
    f32 GetElapsedSeconds() const { return elapsed_seconds; }

  private:
    std::chrono::high_resolution_clock::time_point prev_tick =
        std::chrono::high_resolution_clock::now();
    f32 elapsed_seconds = 0.f;
};

class Tetris3DApp {
  public:
    void Run() {
        using namespace std::literals::chrono_literals;

        if (!StartUp()) {
            return;
        }
        if (!game.StartUp()) {
            return;
        }

        // NOTE(panmar): Even if we set glfwSetFramebufferSizeCallback callback
        // during Tetris3DApp::StartUp(), for some reason it is not triggered
        // when framebuffer is initialized, so, at the beginning, we need to
        // trigger it manually
        {
            i32 framebuffer_width, framebuffer_height;
            glfwGetFramebufferSize(window, &framebuffer_width,
                                   &framebuffer_height);
            game.OnFramebufferResize(framebuffer_width, framebuffer_height);
        }

        timer.Tick();
        while (!glfwWindowShouldClose(window)) {
            input.Update();
            glfwPollEvents();
            timer.Tick();
            game.Update(input, timer.GetElapsedSeconds());

            // TODO(panmar): Enable if exit on lost
            // if (game.IsFinished()) {
            //     glfwSetWindowShouldClose(window, GLFW_TRUE);
            // }

            game.Draw();
            glfwSwapBuffers(window);

            std::this_thread::sleep_for(16ms);
        }
        ShutDown();
    }

    void OnKeyChanged(i32 key, i32 scancode, i32 action, i32 mods) {
        input.OnKeyChanged(key, scancode, action, mods);
    }

    void OnFramebufferResize(i32 width, i32 height) {
        game.OnFramebufferResize(width, height);
    }

  private:
    const char* window_title = "Tetris3d";
    GLFWmonitor* monitor = nullptr;
    GLFWwindow* window = nullptr;
    Timer timer;
    InputState input;
    Game game;

    bool StartUp() {
        srand(time(nullptr));
        glfwInit();

        if (Settings::graphics_renderer_type == Settings::RendererType::Basic) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        } else {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }

        glfwWindowHint(GLFW_SAMPLES, Settings::graphics_multisampling_samples);

        if (Settings::graphics_fullscreen && Settings::graphics_borderless) {
            monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);
            glfwWindowHint(GLFW_RED_BITS, video_mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, video_mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, video_mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, video_mode->refreshRate);
            window = glfwCreateWindow(video_mode->width, video_mode->height,
                                      window_title, monitor, nullptr);
        } else if (Settings::graphics_fullscreen) {
            window =
                glfwCreateWindow(Settings::graphics_resolution_width,
                                 Settings::graphics_resolution_height,
                                 "Tetris3d", glfwGetPrimaryMonitor(), nullptr);
        } else {
            window = glfwCreateWindow(Settings::graphics_resolution_width,
                                      Settings::graphics_resolution_height,
                                      "Tetris3d", nullptr, nullptr);
        }

        if (!window) {
            printf("Failed to create GLFW window\n");
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            printf("Failed to initialize GLAD\n");
            return false;
        }

        // NOTE(panmar): GLFW library uses C-style global callbacks mechanism;
        // We pass App ptr here, so we can access it during global callbacks
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, ::OnFramebufferResize);
        glfwSetKeyCallback(window, OnKeyCallback);

        return true;
    }

    void ShutDown() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};