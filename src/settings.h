#pragma once

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "common.h"

namespace Settings {

const i32 key_playground_rotate_left = GLFW_KEY_LEFT;
const i32 key_playground_rotate_right = GLFW_KEY_RIGHT;
const i32 key_playground_rotate_up = GLFW_KEY_UP;
const i32 key_playground_rotate_down = GLFW_KEY_DOWN;
const i32 key_playground_zoom_in = GLFW_KEY_COMMA;
const i32 key_playground_zoom_out = GLFW_KEY_PERIOD;
const i32 key_block_vert_rot_away = GLFW_KEY_W;
const i32 key_block_vert_rot_towards = GLFW_KEY_S;
const i32 key_block_horiz_rot_clock = GLFW_KEY_A;
const i32 key_block_horiz_rot_counterclock = GLFW_KEY_D;
const i32 key_block_move_away = GLFW_KEY_Q;
const i32 key_block_move_towards = GLFW_KEY_E;
const i32 key_block_accelerate = GLFW_KEY_SPACE;

// const u32 map_width = 10;
// const u32 map_depth = 10;
// const u32 map_height = 50;

const u32 map_width = 7;
const u32 map_depth = 7;
const u32 map_height = 18;


// const f32 block_init_fall_step_seconds = 1 / 5.f;
const f32 block_init_fall_step_seconds = 1 / 1.f;
const f32 block_max_fall_step_seconds = 1 / 25.f;
const f32 block_speed_inc_multiplier = 0.02f;
const f32 block_speed_inc_period_seconds = 10.f;

}; // namespace Settings