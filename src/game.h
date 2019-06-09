#pragma once

#include "camera.h"
#include "common.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "input.h"
#include "logic.h"
#include "renderer.h"
#include "settings.h"

#include <chrono>

class Game {
  public:
    bool StartUp() {
        // TODO(panmar): Experiment with those values
        camera.SetPosition(glm::vec3(game_state.board.width,
                                     1.2f * game_state.board.height,
                                     game_state.board.depth));
        camera.SetTarget(glm::vec3(game_state.board.width / 2.f,
                                   game_state.board.height / 2.f,
                                   game_state.board.depth / 2.f));
        camera.SetUp(glm::vec3(0.f, 1.f, 0.f));

        return true;
    }

    void OnFramebufferResize(i32 width, i32 height) {
        renderer.framebuffer_width = width;
        renderer.framebuffer_height = height;
    }

    void Update(const InputState& input, f32 elapsed_seconds) {
        camera_controller.Update(&camera, input);
        GameLogic::Update(game_state, elapsed_seconds, input,
                          camera.GetForward());
    }

    bool IsFinished() const {
        return game_state.phase == GameLogic::GameState::Phase::Lost;
    }

    void Draw() { renderer.Render(game_state, camera); }

  private:
    GameLogic::GameState game_state;
    PerspectiveCamera camera;
    OrbitCameraController camera_controller;
    GameRenderer renderer;
};
