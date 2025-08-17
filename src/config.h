#ifndef TETRIS3D_CONFIG_H_
#define TETRIS3D_CONFIG_H_

#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

#include <cstdint>
#include <string>

#include "error.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_normalized_axis.hpp"

namespace tetris3d {

struct Config {
    std::string window_title = "tetris3d";
    unsigned rand_seed = 42;

    enum class RendererType { Basic, Advanced };
    const RendererType graphics_renderer_type = RendererType::Advanced;
    int graphics_resolution_width = 1280;
    int graphics_resolution_height = 720;
    bool graphics_fullscreen = true;
    bool graphics_borderless = true;
    bool graphics_multisampling = true;
    bool graphics_multisampling_samples = 4;

    float camera_zoom_min_fov = glm::pi<float>() / 8.f;
    float camera_zoom_max_fov = 4.f * glm::pi<float>() / 5.f;

    int key_camera_rotate_left = GLFW_KEY_LEFT;
    int key_camera_rotate_right = GLFW_KEY_RIGHT;
    int key_camera_rotate_up = GLFW_KEY_UP;
    int key_camera_rotate_down = GLFW_KEY_DOWN;
    int key_camera_zoom_in = GLFW_KEY_COMMA;
    int key_camera_zoom_out = GLFW_KEY_PERIOD;
    int key_camera_center = GLFW_KEY_C;
    int key_block_vert_rot_away = GLFW_KEY_W;
    int key_block_vert_rot_towards = GLFW_KEY_S;
    int key_block_horiz_rot_clock = GLFW_KEY_A;
    int key_block_horiz_rot_counterclock = GLFW_KEY_D;
    int key_block_move_away = GLFW_KEY_Q;
    int key_block_move_towards = GLFW_KEY_E;
    int key_block_accelerate = GLFW_KEY_SPACE;
    int key_quit = GLFW_KEY_ESCAPE;
    int key_pause = GLFW_KEY_P;

    int map_width = 7;
    int map_depth = 7;
    int map_height = 18;

    float block_init_fall_step_seconds = 1 / 2.f;
    float block_max_fall_step_seconds = 1 / 25.f;
    float block_speed_inc_multiplier = 0.02f;
    float block_speed_inc_period_seconds = 10.f;

    static inline constexpr int kMaximumWindowTitleLength = 50;
};

void ValidateConfig(const Config& config);

} // namespace tetris3d

#endif // TETRIS3D_CONFIG_H_