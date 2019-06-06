#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <stdio.h>

#pragma once

class Tetris3D {
  public:
    void Run() {
        if (!StartUp()) {
            return;
        }
        while (!glfwWindowShouldClose(window)) {
            Update();
            Draw();
        }
        ShutDown();
    }

  private:
    GLFWwindow *window = nullptr;

    bool StartUp() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(800, 600, "Tetris3d", nullptr, nullptr);
        if (!window) {
            printf("Failed to create GLFW window\n");
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(window);
        return true;
    }

    void ShutDown() { glfwTerminate(); }

    void Update() {}

    void Draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
};