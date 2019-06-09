#pragma once

#include "camera.h"
#include "common.h"
#include "logic.h"

#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

#include "glm/vec3.hpp"

class IRenderer {
  public:
    ~IRenderer() = default;
    virtual void Render(const GameLogic::GameState& state,
                        const Camera& camera) = 0;
    virtual void SetFramebufferWidth(i32 value) = 0;
    virtual void SetFramebufferHeight(i32 value) = 0;
};

class BasicRenderer : public IRenderer {
  public:
    void Render(const GameLogic::GameState& state,
                const Camera& camera) override;

    void RenderBoard(const GameLogic::Board3D& board);
    void RenderFallingBlock(const GameLogic::Block& block);
    void RenderFallingBlockProjection(const GameLogic::GameState& state);

    void RenderGizmo(const glm::vec3& position);

    void RenderBounds(const GameLogic::Board3D& board,
                      const glm::vec3& view_dir);

    void RenderCube(const glm::vec3& position, const Color& color, f32 width,
                    f32 depth, f32 height);

    void RenderTetrisCube(const glm::vec3& pos, const Color& color);

    void SetFramebufferWidth(i32 value) override { framebuffer_width = value; }
    void SetFramebufferHeight(i32 value) override {
        framebuffer_height = value;
    }

    i32 framebuffer_width = 0;
    i32 framebuffer_height = 0;
};