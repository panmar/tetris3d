#pragma once

#include <string>

#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"
#include "glm/vec3.hpp"

#include "config.h"
#include "game/camera.h"
#include "game/board.h"
#include "game/state.h"

namespace tetris3d {

class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void StartUp(const GameState& state) {};
    virtual void Render(const GameState& state,
                        const PerspectiveCamera& camera) = 0;
    virtual void SetFramebufferWidth(int value) = 0;
    virtual void SetFramebufferHeight(int value) = 0;
};

// OpenGL 2.1 renderer
class BasicRenderer : public IRenderer {
public:
    ~BasicRenderer() {}
    BasicRenderer(Config&);
    void Render(const GameState& state,
                const PerspectiveCamera& camera) override;

    void RenderBoard(const Board& board);
    void RenderFallingBlock(const Block& block);
    void RenderFallingBlockProjection(const GameState& state);

    void RenderGizmo(const glm::vec3& position);

    void RenderBounds(const Board& board, const glm::vec3& view_dir);

    void RenderCube(const glm::vec3& position, const Color& color, float width,
                    float depth, float height);

    void RenderTetrisCube(const glm::vec3& pos, const Color& color);

    void SetFramebufferWidth(int value) override { framebuffer_width_ = value; }
    void SetFramebufferHeight(int value) override {
        framebuffer_height_ = value;
    }

private:
    Config& config_;
    int framebuffer_width_ = 0;
    int framebuffer_height_ = 0;
};

class Shader {
public:
    ~Shader();
    void Create(const std::string& vs_str, const std::string& fs_str);
    void SetParam(const char* name, bool value) const;
    void SetParam(const char* name, int value) const;
    void SetParam(const char* name, float value) const;
    void SetParam(const char* name, const glm::vec2& value) const;
    void SetParam(const char* name, float x, float y) const;
    void SetParam(const char* name, const glm::vec3& value) const;
    void SetParam(const char* name, float x, float y, float z) const;
    void SetParam(const char* name, const glm::vec4& value) const;
    void SetParam(const char* name, float x, float y, float z, float w) const;
    void SetParam(const char* name, const glm::mat2& value) const;
    void SetParam(const char* name, const glm::mat3& value) const;
    void SetParam(const char* name, const glm::mat4& value) const;
    void SetParam(const char* name, const Color& color) const;
    void Bind();

private:
    bool LogShaderErrorsIfAny(unsigned shader);
    bool LogProgramErrorsIfAny(unsigned shader);
    unsigned id_ = 0;
};

class SkyBox {
public:
    ~SkyBox();
    void Create();
    void Render(Shader& shader, const Camera& camera);

    // Load order: X, -X, Y, -Y, Z, -Z
    unsigned LoadCubemap(const std::vector<std::string>& paths);

    Color color = Color{1.f, 1.f, 1.f, 1.f};
    unsigned vao = 0;
    unsigned vbo = 0;
    unsigned texture = 0;
};

class Cube {
public:
    void Create(float width, float depth, float height);
    ~Cube();
    void Render(Shader& shader, const Color& color, const glm::mat4& world,
                const Camera& camera);
    void RenderRefract(Shader& shader, const Color& color, const SkyBox& skybox,
                       const glm::mat4& world, const Camera& camera);

private:
    unsigned vbo_ = 0;
    unsigned vao_ = 0;
};

class BoardBounds {
public:
    ~BoardBounds();
    void Create(const Board& board);
    void Render(Shader& shader, const Color& color, const Camera& camera);

private:
    unsigned vbo_wall_ = 0;
    unsigned vao_wall_ = 0;
    unsigned vbo_ground_ = 0;
    unsigned vao_ground_ = 0;
    unsigned ground_vertex_count_ = 0;
    unsigned wall_vertex_count_ = 0;
    glm::ivec3 board_dimensions_;
};

// OpenGL 3.3 renderer
class AdvancedRenderer : public IRenderer {
public:
    AdvancedRenderer(Config&);
    ~AdvancedRenderer() {}
    void StartUp(const GameState& state) override;
    void Render(const GameState& state,
                const PerspectiveCamera& camera) override;
    void SetFramebufferWidth(int value) override { framebuffer_width_ = value; }
    void SetFramebufferHeight(int value) override {
        framebuffer_height_ = value;
    }

private:
    void RenderBoard(const Board& board, const Camera& camera);
    void RenderFallingBlock(const Block& block, const Camera& camera);
    void RenderFallingBlockProjection(const GameState& state,
                                      const Camera& camera);
    void RenderTetrisCube(const glm::vec3& pos, const Color& color,
                          const Camera& camera);
    void RenderCube(const glm::vec3& position, const Color& color,
                    const glm::vec3& scale, const Camera& camera);

    void RenderRefractCube(const glm::vec3& position, const Color& color,
                           const glm::vec3& scale, const Camera& camera);

    Config config_;
    int framebuffer_width_ = 0;
    int framebuffer_height_ = 0;

    Shader solid_shader_;
    Shader solid_wire_shader_;
    Shader refract_shader_;
    Shader skybox_shader_;
    Cube tetris_cube_;
    BoardBounds board_bounds_;
    SkyBox skybox_;
};

} // namespace tetris3d
