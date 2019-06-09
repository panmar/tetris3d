#pragma once

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <chrono>
#include <stdio.h>
#include <thread>

#include "game.h"
#include "input.h"

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
    GLFWwindow* window = nullptr;
    Timer timer;
    InputState input;
    Game game;

    bool StartUp() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(800, 600, "Tetris3d", nullptr, nullptr);
        if (!window) {
            printf("Failed to create GLFW window\n");
            glfwTerminate();
            return false;
        }

        // NOTE(panmar): GLFW library uses C-style global callbacks mechanism;
        // We pass App ptr here, so we can access it during global callbacks
        glfwSetWindowUserPointer(window, this);

        // NOTE(panmar): Even if we set callback here, it will not be
        // triggered when framebuffer is initialized, so, at the beginning,
        // we need to trigger it manually
        glfwSetFramebufferSizeCallback(window, ::OnFramebufferResize);
        i32 framebuffer_width, framebuffer_height;
        glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
        game.OnFramebufferResize(framebuffer_width, framebuffer_height);

        glfwSetKeyCallback(window, OnKeyCallback);

        glfwMakeContextCurrent(window);
        return true;
    }

    void ShutDown() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};