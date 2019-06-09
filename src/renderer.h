#pragma once

#include "camera.h"
#include "common.h"
#include "logic.h"

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "glm/vec3.hpp"

inline void DrawGizmo(const glm::vec3& position) {
    glBegin(GL_LINES);

    auto edge = 1.f;

    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(position.x, position.y, position.z);
    glVertex3f(position.x + edge, position.y, position.z);

    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(position.x, position.y, position.z);
    glVertex3f(position.x, position.y + edge, position.z);

    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(position.x, position.y, position.z);
    glVertex3f(position.x, position.y, position.z + edge);

    glEnd();
}

inline void DrawBounds(const GameLogic::Board3D& board,
                       const glm::vec3& view_dir) {
    glBegin(GL_LINES);

    glColor3f(0.25f, 0.25f, 0.25f);

    // floor rendering
    for (auto i = 0.f; i < board.width + 0.1f; i += 1.f) {
        glVertex3f(i, 0.f, 0.f);
        glVertex3f(i, 0.f, board.depth);
        glVertex3f(0.f, 0.f, i);
        glVertex3f(board.width, 0.f, i);
    }

    {
        glm::vec3 wall_dir{0, 0, 1};
        if (glm::dot(view_dir, wall_dir) < 0.f) {
            for (auto i = 0.f; i < board.width + 0.1f; i += 1.f) {
                glVertex3f(i, 0.f, 0.f);
                glVertex3f(i, board.height, 0.f);
            }

            for (auto i = 1.f; i < board.height + 0.1f; i += 1.f) {
                glVertex3f(0.f, i, 0.f);
                glVertex3f(board.width, i, 0.f);
            }
        }
    }

    {
        glm::vec3 wall_dir{1, 0, 0};
        if (glm::dot(view_dir, wall_dir) < 0.f) {
            for (auto i = 0.f; i < board.width + 0.1f; i += 1.f) {
                glVertex3f(0.f, 0.f, i);
                glVertex3f(0.f, board.height, i);
            }

            for (auto i = 1.f; i < board.height + 0.1f; i += 1.f) {
                glVertex3f(0.f, i, 0.f);
                glVertex3f(0.f, i, board.depth);
            }
        }
    }

    {
        glm::vec3 wall_dir{0, 0, -1};
        if (glm::dot(view_dir, wall_dir) < 0.f) {
            for (auto i = 0.f; i < board.width + 0.1f; i += 1.f) {
                glVertex3f(i, 0.f, board.depth);
                glVertex3f(i, board.height, board.depth);
            }

            for (auto i = 1.f; i < board.height + 0.1f; i += 1.f) {
                glVertex3f(board.width, i, board.depth);
                glVertex3f(0.f, i, board.depth);
            }
        }
    }

    {
        glm::vec3 wall_dir{-1, 0, 0};
        if (glm::dot(view_dir, wall_dir) < 0.f) {
            for (auto i = 0.f; i < board.width + 0.1f; i += 1.f) {
                glVertex3f(board.width, 0.f, i);
                glVertex3f(board.width, board.height, i);
            }

            for (auto i = 1.f; i < board.height + 0.1f; i += 1.f) {
                glVertex3f(board.width, i, board.depth);
                glVertex3f(board.width, i, 0.f);
            }
        }
    }

    glEnd();
}

inline void DrawCube(const glm::vec3& position, const Color& color, f32 width,
                     f32 depth, f32 height) {
    glBegin(GL_TRIANGLES);
    glColor4f(color.r, color.g, color.b, color.a);

    auto wOver2 = width / 2.f;
    auto hOver2 = height / 2.f;
    auto dOver2 = depth / 2.f;
    glm::vec3 positions[] = {
        {-wOver2, -hOver2, -dOver2}, // 0
        {-wOver2, hOver2, -dOver2},  // 1
        {wOver2, hOver2, -dOver2},   // 2
        {-wOver2, -hOver2, -dOver2}, {wOver2, hOver2, -dOver2},
        {wOver2, -hOver2, -dOver2}, // 3
        {wOver2, -hOver2, -dOver2},  {wOver2, hOver2, -dOver2},
        {wOver2, -hOver2, dOver2}, // 4
        {wOver2, -hOver2, dOver2},   {wOver2, hOver2, -dOver2},
        {wOver2, hOver2, dOver2}, // 5
        {wOver2, hOver2, -dOver2},   {-wOver2, hOver2, -dOver2},
        {-wOver2, hOver2, dOver2}, // 6
        {wOver2, hOver2, -dOver2},   {-wOver2, hOver2, dOver2},
        {wOver2, hOver2, dOver2},    {wOver2, -hOver2, dOver2},
        {-wOver2, hOver2, dOver2},   {-wOver2, -hOver2, dOver2}, // 7
        {wOver2, -hOver2, dOver2},   {wOver2, hOver2, dOver2},
        {-wOver2, hOver2, dOver2},   {-wOver2, -hOver2, dOver2},
        {-wOver2, hOver2, -dOver2},  {-wOver2, -hOver2, -dOver2},
        {-wOver2, -hOver2, dOver2},  {-wOver2, hOver2, dOver2},
        {-wOver2, hOver2, -dOver2},  {-wOver2, -hOver2, -dOver2},
        {wOver2, -hOver2, -dOver2},  {-wOver2, -hOver2, dOver2},
        {wOver2, -hOver2, -dOver2},  {wOver2, -hOver2, dOver2},
        {-wOver2, -hOver2, dOver2},
    };

    for (auto& pos : positions) {
        pos += position;
    }

    auto positions_count = sizeof(positions) / sizeof(positions[0]);

    for (size_t i = 0; i < positions_count; ++i) {
        glVertex3f(positions[i].x, positions[i].y, positions[i].z);
    }
    glEnd();
}

inline void DrawGameCube(const glm::vec3& pos, const Color& color) {
    Color cube_edge_color{0.1f, 0.1f, 0.1f};
    DrawCube(pos, cube_edge_color, 1.0f, 1.0f, 1.0f);
    DrawCube(pos, color, 1.01f, 0.8f, 0.8f);
    DrawCube(pos, color, 0.8f, 1.01f, 0.8f);
    DrawCube(pos, color, 0.8f, 0.8f, 1.01f);
}

class GameRenderer {
  public:
    void Render(const GameLogic::GameState& state, const Camera& camera) {
        glViewport(0, 0, framebuffer_width, framebuffer_height);

        if (state.phase == GameLogic::GameState::Phase::Lost) {
            glClearColor(0.2f, 0.1f, 0.1f, 1.f);
        } else if (state.phase == GameLogic::GameState::Phase::LayersErase) {
            glClearColor(0.9f, 0.9f, 0.9f, 1.f);
        } else {
            glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        }

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DrawBounds(state.board, camera.GetForward());
        DrawGizmo(glm::vec3(state.board.width / 2.f, state.board.height / 2.f,
                            state.board.depth / 2.f));

        RenderBoard(state.board);

        RenderFallingBlock(state.falling_block);
        if (state.phase == GameLogic::GameState::Phase::BlockFalling) {
            RenderFallingBlockProjection(state);
        }
    }

    void RenderBoard(const GameLogic::Board3D& board) {
        for (size_t k = 0; k < board.height; ++k) {
            for (size_t i = 0; i < board.width; ++i) {
                for (size_t j = 0; j < board.depth; ++j) {
                    auto index =
                        (i * board.width + j) + (k * board.width * board.depth);
                    if (board.cells[index]) {
                        auto position = glm::vec3(i, k, j);
                        Color color =
                            FromPackedColorToColor(board.cells[index]);
                        DrawGameCube(position + glm::vec3(0.5f, 0.5f, 0.5f),
                                     color);
                    }
                }
            }
        }
    }

    void RenderFallingBlock(const GameLogic::Block& block) {
        Color color;
        color.r = block.color.r / 255.f;
        color.g = block.color.g / 255.f;
        color.b = block.color.b / 255.f;

        for (auto& offset : block.cube_offsets) {
            DrawGameCube(glm::vec3(block.position + offset) +
                             glm::vec3(0.5f, 0.5f, 0.5f),
                         color);
        }
    }

    void RenderFallingBlockProjection(const GameLogic::GameState& state) {
        auto projected_block = state.falling_block;
        while (projected_block.IsValid(state.board)) {
            projected_block.Translate(glm::ivec3(0, -1, 0));
        }
        projected_block.Translate(glm::ivec3(0, 1, 0));

        Color color{0.9, 0.9f, 0.9f, 0.6f};

        glColorMask(false, false, false, false);
        for (auto& offset : projected_block.cube_offsets) {
            DrawCube(glm::vec3(projected_block.position + offset) +
                         glm::vec3(0.5f, 0.5f, 0.5f),
                     color, 1.f, 1.f, 1.f);
        }

        glColorMask(true, true, true, true);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // glDepthMask(false);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (auto& offset : projected_block.cube_offsets) {
            DrawCube(glm::vec3(projected_block.position + offset) +
                         glm::vec3(0.5f, 0.5f, 0.5f),
                     color, 1.f, 1.f, 1.f);
        }
        glDisable(GL_BLEND);
        // glDepthMask(true);
        glDepthFunc(GL_LESS);
    }

    i32 framebuffer_width = 0;
    i32 framebuffer_height = 0;
};