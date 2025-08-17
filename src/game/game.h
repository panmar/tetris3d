#pragma once

#include <chrono>
#include <memory>

#include "game/camera.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "input.h"
#include "renderer.h"
#include "config.h"
#include "game/camera_controller.h"

namespace tetris3d {

class Game {
public:
    Game(Config& config);

    bool StartUp();
    void OnFramebufferResize(int width, int height);
    void Update(const Input& input, float elapsed_seconds);
    void CenterCamera(Camera& camera);
    bool IsFinished() const;
    void Draw();

private:
    // Time-aware update
    void Update(GameState& state, float elapsed_seconds, const Input& input,
                const glm::vec3& view_dir);

    // Time-agnostic update
    void SingleStep(GameState& state);
    bool CanFallingBlockFall(const GameState& state);
    void MergeFallingBlock(GameState& state);

    Config& config_;
    GameState state_;
    OrbitCameraController camera_controller_;
    std::unique_ptr<IRenderer> renderer_;
};

} // namespace tetris3d
