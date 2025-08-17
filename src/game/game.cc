#include "game/game.h"

namespace tetris3d {

Game::Game(Config& config)
    : config_(config), state_(config_), camera_controller_(config_) {}

bool Game::StartUp() {
    if (config_.graphics_renderer_type == Config::RendererType::Basic) {
        renderer_ = std::make_unique<BasicRenderer>(config_);
    } else {
        renderer_ = std::make_unique<AdvancedRenderer>(config_);
    }
    renderer_->StartUp(state_);

    CenterCamera(state_.camera);
    state_.camera.SetAspectRatio(
        config_.graphics_resolution_width /
        static_cast<float>(config_.graphics_resolution_height));

    return true;
}

void Game::OnFramebufferResize(int width, int height) {
    renderer_->SetFramebufferWidth(width);
    renderer_->SetFramebufferHeight(height);
}

void Game::Update(const Input& input, float elapsed_seconds) {
    if (input.IsKeyPressed(config_.key_camera_center)) {
        CenterCamera(state_.camera);
    }

    camera_controller_.Update(&state_.camera, input);
    Update(state_, elapsed_seconds, input, state_.camera.GetForward());
}

void Game::CenterCamera(Camera& camera) {
    camera.SetPosition(glm::vec3(2.5f * state_.board.GetWidth(),
                                 state_.board.GetHeight() / 2.f,
                                 2.5f * state_.board.GetDepth()));
    camera.SetTarget(glm::vec3(state_.board.GetWidth() / 2.f,
                               state_.board.GetHeight() / 2.f,
                               state_.board.GetDepth() / 2.f));
    camera.SetUp(glm::vec3(0.f, 1.f, 0.f));
}

bool Game::IsFinished() const { return state_.phase == GameState::Phase::Lost; }

void Game::Draw() { renderer_->Render(state_, state_.camera); }

void Game::Update(GameState& state, float elapsed_seconds, const Input& input,
                  const glm::vec3& view_dir) {

    if (input.IsKeyPressed(config_.key_pause)) {
        state.paused = !state.paused;
    }

    if (state.phase == GameState::Phase::Lost || state.paused) {
        return;
    }

    if (input.IsKeyPressed(config_.key_block_horiz_rot_clock)) {
        if (view_dir.y < 0.f) {
            state.falling_block.TryRotateYClockwiseWithFix(state.board);
        } else {
            state.falling_block.TryRotateYCounterClockwiseWithFix(state.board);
        }
    }
    if (input.IsKeyPressed(config_.key_block_horiz_rot_counterclock)) {
        if (view_dir.y < 0.f) {
            state.falling_block.TryRotateYCounterClockwiseWithFix(state.board);
        } else {
            state.falling_block.TryRotateYClockwiseWithFix(state.board);
        }
    }

    if (input.IsKeyPressed(config_.key_block_vert_rot_away)) {
        auto view_dir_xz = view_dir;
        view_dir_xz.y = 0.f;
        view_dir_xz = glm::normalize(view_dir_xz);
        auto dot = glm::dot(glm::vec3(1.f, 0.f, 0.f), view_dir_xz);
        if (glm::abs(dot) < glm::cos(glm::pi<float>() / 4.f)) {
            if (view_dir.z < 0.f) {
                state.falling_block.TryRotateXClockwiseWithFix(state.board);
            } else {
                state.falling_block.TryRotateXCounterClockwiseWithFix(
                    state.board);
            }
        } else {
            if (view_dir.x < 0.f) {
                state.falling_block.TryRotateZCounterClockwiseWithFix(
                    state.board);
            } else {
                state.falling_block.TryRotateZClockwiseWithFix(state.board);
            }
        }
    }

    if (input.IsKeyPressed(config_.key_block_vert_rot_towards)) {
        auto view_dir_xz = view_dir;
        view_dir_xz.y = 0.f;
        view_dir_xz = glm::normalize(view_dir_xz);
        auto dot = glm::dot(glm::vec3(1.f, 0.f, 0.f), view_dir_xz);
        if (glm::abs(dot) < glm::cos(glm::pi<float>() / 4.f)) {
            if (view_dir.z > 0.f) {
                state.falling_block.TryRotateXClockwiseWithFix(state.board);
            } else {
                state.falling_block.TryRotateXCounterClockwiseWithFix(
                    state.board);
            }
        } else {
            if (view_dir.x > 0.f) {
                state.falling_block.TryRotateZCounterClockwiseWithFix(
                    state.board);
            } else {
                state.falling_block.TryRotateZClockwiseWithFix(state.board);
            }
        }
    }

    if (input.IsKeyPressed(config_.key_block_move_away)) {
        auto view_dir_xz = view_dir;
        view_dir_xz.y = 0.f;
        view_dir_xz = glm::normalize(view_dir_xz);
        auto dot = glm::dot(glm::vec3(1.f, 0.f, 0.f), view_dir_xz);
        if (glm::abs(dot) < glm::cos(glm::pi<float>() / 4.f)) {
            if (view_dir.z < 0.f) {
                state.falling_block.TryTranslate(state.board,
                                                 glm::ivec3(0, 0, -1));
            } else {
                state.falling_block.TryTranslate(state.board,
                                                 glm::ivec3(0, 0, 1));
            }
        } else {
            if (view_dir.x < 0.f) {
                state.falling_block.TryTranslate(state.board,
                                                 glm::ivec3(-1, 0, 0));
            } else {
                state.falling_block.TryTranslate(state.board,
                                                 glm::ivec3(1, 0, 0));
            }
        }
    }

    if (input.IsKeyPressed(config_.key_block_move_towards)) {
        auto view_dir_xz = view_dir;
        view_dir_xz.y = 0.f;
        view_dir_xz = glm::normalize(view_dir_xz);
        auto dot = glm::dot(glm::vec3(1.f, 0.f, 0.f), view_dir_xz);
        if (glm::abs(dot) < glm::cos(glm::pi<float>() / 4.f)) {
            if (view_dir.z > 0.f) {
                state.falling_block.TryTranslate(state.board,
                                                 glm::ivec3(0, 0, -1));
            } else {
                state.falling_block.TryTranslate(state.board,
                                                 glm::ivec3(0, 0, 1));
            }
        } else {
            if (view_dir.x > 0.f) {
                state.falling_block.TryTranslate(state.board,
                                                 glm::ivec3(-1, 0, 0));
            } else {
                state.falling_block.TryTranslate(state.board,
                                                 glm::ivec3(1, 0, 0));
            }
        }
    }

    if (input.IsKeyDown(config_.key_block_accelerate)) {
        state.block_current_speed = state.block_max_fall_step_seconds;
        // NOTE(panmar): Let the fall pace change be immidiate
        state.seconds_to_next_block_fall = std::min(
            state.seconds_to_next_block_fall, state.block_current_speed);
    } else {
        state.block_current_speed = state.block_current_normal_speed;
    }

    state.seconds_to_next_block_fall -= elapsed_seconds;
    state.seconds_from_last_speed_inc -= elapsed_seconds;

    // Game logic tick
    if (state.seconds_to_next_block_fall < 0.f) {
        SingleStep(state);
        // NOTE(panmar): Reset next fall counter if we have been falling
        // during this step; it is possible other action ocurred (block merging,
        // layer disapper, etc.)
        if (state.phase == GameState::Phase::BlockFalling) {
            state.seconds_to_next_block_fall = state.block_current_speed;
        }
    }

    // Speed increase logic
    if (state.seconds_from_last_speed_inc < 0.f) {
        state.block_current_normal_speed += (state.block_speed_inc_multiplier *
                                             state.block_current_normal_speed);
        state.seconds_from_last_speed_inc =
            state.block_speed_inc_period_seconds;
    }

    state.total_time += elapsed_seconds;
}

void Game::SingleStep(GameState& state) {
    if (state.phase == GameState::Phase::Lost) {
        return;
    }

    if (state.phase == GameState::Phase::Uninitialized) {
        state.falling_block = Block::CreateRandom(state.board);
        state.phase = GameState::Phase::NewBlockCreation;
        return;
    }

    if (state.phase == GameState::Phase::BlockMerge) {
        if (state.board.EraseFilledLayers()) {
            state.phase = GameState::Phase::LayersErase;
            return;
        }
    }

    if (state.phase == GameState::Phase::BlockMerge ||
        state.phase == GameState::Phase::LayersErase) {
        state.falling_block = Block::CreateRandom(state.board);
        state.phase = GameState::Phase::NewBlockCreation;
        return;
    }

    if (!CanFallingBlockFall(state)) {
        if (!state.falling_block.IsValid(state.board)) {
            state.phase = GameState::Phase::Lost;
            return;
        }
        MergeFallingBlock(state);
        state.phase = GameState::Phase::BlockMerge;
        return;
    } else {
        state.falling_block.Translate(glm::ivec3(0, -1, 0));
        state.phase = GameState::Phase::BlockFalling;
        return;
    }
}

bool Game::CanFallingBlockFall(const GameState& state) {
    for (auto& cube_offset : state.falling_block.GetCubeOffsets()) {
        auto absolute_pos = state.falling_block.GetPosition() + cube_offset;
        auto final_pos = absolute_pos - glm::ivec3(0, 1, 0);
        // TODO(panmar): For now, do not consider starting position
        if (final_pos.y >= 50)
            continue;
        if (final_pos.y < 0 || !state.board.IsEmpty(final_pos)) {
            return false;
        }
    }
    for (auto& cube_offset : state.falling_block.GetCubeOffsets()) {
        auto absolute_pos = state.falling_block.GetPosition() + cube_offset;
        auto final_pos = absolute_pos - glm::ivec3(0, -1, 0);
        assert(final_pos.y >= 0);
    }

    return true;
}

void Game::MergeFallingBlock(GameState& state) {
    for (auto& cube_offset : state.falling_block.GetCubeOffsets()) {
        auto absolute_pos = state.falling_block.GetPosition() + cube_offset;
        state.board.Fill(absolute_pos,
                         PackColor(state.falling_block.GetColor()));
    }
}

} // namespace tetris3d