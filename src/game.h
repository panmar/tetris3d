#pragma once

#include <chrono>
#include <memory>

#include "camera.h"
#include "common.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "input.h"
#include "logic.h"
#include "renderer.h"
#include "settings.h"

class Game {
  public:
    bool StartUp() {
        if (Settings::graphics_renderer_type == Settings::RendererType::Basic) {
            renderer = std::make_unique<BasicRenderer>();
        } else {
            renderer = std::make_unique<AdvancedRenderer>();
        }

        // TODO(panmar): Experiment with those values
        camera.SetPosition(glm::vec3(game_state.board.width,
                                     1.2f * game_state.board.height,
                                     game_state.board.depth));
        camera.SetTarget(glm::vec3(game_state.board.width / 2.f,
                                   game_state.board.height / 2.f,
                                   game_state.board.depth / 2.f));
        camera.SetUp(glm::vec3(0.f, 1.f, 0.f));
        camera.SetAspectRatio(
            Settings::graphics_resolution_width /
            static_cast<f32>(Settings::graphics_resolution_height));

        return true;
    }

    void OnFramebufferResize(i32 width, i32 height) {
        renderer->SetFramebufferWidth(width);
        renderer->SetFramebufferHeight(height);
    }

    void Update(const InputState& input, f32 elapsed_seconds) {
        camera_controller.Update(&camera, input);
        GameLogic::Update(game_state, elapsed_seconds, input,
                          camera.GetForward());
    }

    bool IsFinished() const {
        return game_state.phase == GameLogic::GameState::Phase::Lost;
    }

    void Draw() { renderer->Render(game_state, camera); }

  private:
    GameLogic::GameState game_state;
    PerspectiveCamera camera;
    OrbitCameraController camera_controller;
    std::unique_ptr<IRenderer> renderer;
};
