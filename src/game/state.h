#ifndef TETRIS3D_GAME_STATE_H_
#define TETRIS3D_GAME_STATE_H_

#include "config.h"
#include "game/block.h"
#include "game/board.h"
#include "game/camera.h"

namespace tetris3d {

struct GameState {
    GameState(const Config& config)
        : board(config.map_width, config.map_depth, config.map_height),
          block_current_speed(config.block_init_fall_step_seconds),
          block_current_normal_speed(config.block_init_fall_step_seconds),
          block_max_fall_step_seconds(config.block_max_fall_step_seconds),
          block_speed_inc_multiplier(config.block_speed_inc_multiplier),
          block_speed_inc_period_seconds(config.block_speed_inc_period_seconds),
          seconds_from_last_speed_inc(config.block_speed_inc_period_seconds),
          seconds_to_next_block_fall(config.block_init_fall_step_seconds) {}

    Board board;
    Block falling_block;
    PerspectiveCamera camera;

    enum class Phase {
        Uninitialized,
        NewBlockCreation,
        BlockFalling,
        BlockMerge,
        LayersErase,
        Lost
    };

    Phase phase = Phase::Uninitialized;

    bool paused = false;

    // current falling block speed - can be normal or maximum (if accelerated)
    float block_current_speed = 0.f;
    // current normal speed (not accelerated)
    float block_current_normal_speed = 0.f;
    const float block_max_fall_step_seconds = 0.f;
    const float block_speed_inc_multiplier = 0.f;
    const float block_speed_inc_period_seconds = 0.f;

    float total_time = 0.f;
    float seconds_from_last_speed_inc = 0.f;
    float seconds_to_next_block_fall = 0.f;
};

} // namespace tetris3d

#endif // TETRIS3D_GAME_STATE_H_