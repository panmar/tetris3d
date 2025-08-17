#pragma once

#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_normalized_axis.hpp"

namespace tetris3d {

namespace Settings {

enum class RendererType { Basic, Advanced };

// const RendererType graphics_renderer_type = RendererType::Basic;
const RendererType graphics_renderer_type = RendererType::Advanced;
const unsigned graphics_resolution_width = 1920;
const unsigned graphics_resolution_height = 1080;
const bool graphics_fullscreen = true;
const bool graphics_borderless = true;
const bool graphics_multisampling = true;
const bool graphics_multisampling_samples = 4;

const float camera_zoom_min_fov = glm::pi<float>() / 8.f;
const float camera_zoom_max_fov = 4.f * glm::pi<float>() / 5.f;

const int key_playground_rotate_left = GLFW_KEY_LEFT;
const int key_playground_rotate_right = GLFW_KEY_RIGHT;
const int key_playground_rotate_up = GLFW_KEY_UP;
const int key_playground_rotate_down = GLFW_KEY_DOWN;
const int key_playground_zoom_in = GLFW_KEY_COMMA;
const int key_playground_zoom_out = GLFW_KEY_PERIOD;
const int key_block_vert_rot_away = GLFW_KEY_W;
const int key_block_vert_rot_towards = GLFW_KEY_S;
const int key_block_horiz_rot_clock = GLFW_KEY_A;
const int key_block_horiz_rot_counterclock = GLFW_KEY_D;
const int key_block_move_away = GLFW_KEY_Q;
const int key_block_move_towards = GLFW_KEY_E;
const int key_block_accelerate = GLFW_KEY_SPACE;
const int key_quit = GLFW_KEY_ESCAPE;
const int key_camera_center = GLFW_KEY_C;
const int key_pause = GLFW_KEY_P;

const unsigned map_width = 7;
const unsigned map_depth = 7;
const unsigned map_height = 18;

const float block_init_fall_step_seconds = 1 / 2.f;
const float block_max_fall_step_seconds = 1 / 25.f;
const float block_speed_inc_multiplier = 0.02f;
const float block_speed_inc_period_seconds = 10.f;

}; // namespace Settings

} // namespace tetris3d
