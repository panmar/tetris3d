#include "logic.h"
#include "glm/geometric.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_normalized_axis.hpp"
#include "glm/vec3.hpp"
#include "settings.h"
#include <climits>

namespace GameLogic {

Block Block::Create(BlockType type, const ColorR8G8B8& color,
                    const Board3D& board) {
    Block block;
    block.type = type;
    block.color = color;
    block.position = glm::ivec3(board.width / 2, board.height, board.depth / 2);

    switch (type) {
    case BlockType::LShape:
        block.cube_offsets.emplace_back(0, 0, 0);
        block.cube_offsets.emplace_back(-1, 0, 0);
        block.cube_offsets.emplace_back(1, 0, 0);
        block.cube_offsets.emplace_back(1, 1, 0);
        // TODO(panmar): block height could be calculated
        block.position -= glm::ivec3(0, 2, 0);
        break;
    case BlockType::IShape:
        block.cube_offsets.emplace_back(0, 0, 0);
        block.cube_offsets.emplace_back(-1, 0, 0);
        block.cube_offsets.emplace_back(-2, 0, 0);
        block.cube_offsets.emplace_back(1, 0, 0);
        block.position -= glm::ivec3(0, 1, 0);
        break;
    case BlockType::JShape:
        block.cube_offsets.emplace_back(0, 0, 0);
        block.cube_offsets.emplace_back(-1, 0, 0);
        block.cube_offsets.emplace_back(1, 0, 0);
        block.cube_offsets.emplace_back(-1, 1, 0);
        block.position -= glm::ivec3(0, 2, 0);
        break;
    case BlockType::OShape:
        block.cube_offsets.emplace_back(0, 0, 0);
        block.cube_offsets.emplace_back(0, 1, 0);
        block.cube_offsets.emplace_back(-1, 0, 0);
        block.cube_offsets.emplace_back(-1, 1, 0);
        block.cube_offsets.emplace_back(0, 0, 1);
        block.cube_offsets.emplace_back(0, 1, 1);
        block.cube_offsets.emplace_back(-1, 0, 1);
        block.cube_offsets.emplace_back(-1, 1, 1);
        block.position -= glm::ivec3(0, 2, 0);
        break;
    case BlockType::SShape:
        block.cube_offsets.emplace_back(0, 0, 0);
        block.cube_offsets.emplace_back(-1, 0, 0);
        block.cube_offsets.emplace_back(0, 1, 0);
        block.cube_offsets.emplace_back(1, 1, 0);
        block.position -= glm::ivec3(0, 2, 0);
        break;
    case BlockType::TShape:
        block.cube_offsets.emplace_back(0, 0, 0);
        block.cube_offsets.emplace_back(-1, 0, 0);
        block.cube_offsets.emplace_back(1, 0, 0);
        block.cube_offsets.emplace_back(0, 1, 0);
        block.position -= glm::ivec3(0, 2, 0);
        break;
    case BlockType::ZShape:
        block.cube_offsets.emplace_back(0, 0, 0);
        block.cube_offsets.emplace_back(0, 1, 0);
        block.cube_offsets.emplace_back(-1, 1, 0);
        block.cube_offsets.emplace_back(1, 0, 0);
        block.position -= glm::ivec3(0, 2, 0);
        break;
    case BlockType::Undefined:
    default:
        assert(false);
    }

    // NOTE(panmar): If we created a block that is colliding with others,
    // we have created losing block; We move it up (even outside of board
    // bounds) untill is does not collide (it can, and probably will be located
    // outside the valid board)
    while (block.IsCollidingWithOtherBlocks(board)) {
        block.Translate(glm::ivec3(0, 1, 0));
    }

    return block;
}

Block Block::CreateRandom(const Board3D& board) {
    u32 block_type_idx = rand() % static_cast<u32>(BlockType::Undefined);
    BlockType type = static_cast<BlockType>(block_type_idx);

    // NOTE(panmar): pure black color is excluded;
    // It would be packed to 0x00000000 and could not be distinquished from
    // empty cell in tetris playground;

    // TODO(panmar): Randomization should probablly be done in more "natural"
    // way; Like having a shuffled vector of predefined colors
    ColorR8G8B8 color{0, 0, 0};
    while (!color.r && !color.b && !color.g) {
        color = ColorR8G8B8{static_cast<u8>(rand() % 256),
                            static_cast<u8>(rand() % 256),
                            static_cast<u8>(rand() % 256)};
    }
    return Block::Create(type, color, board);
}

void Block::Translate(const glm::ivec3& value) { position += value; }

void Block::RotateXClockwise() {
    if (type == BlockType::OShape)
        return;
    for (auto& offset : cube_offsets) {
        offset.y = -offset.y;
        std::swap(offset.y, offset.z);
    }
}

void Block::RotateXCounterClockwise() {
    if (type == BlockType::OShape)
        return;
    for (auto& offset : cube_offsets) {
        offset.z = -offset.z;
        std::swap(offset.y, offset.z);
    }
}

void Block::RotateYClockwise() {
    if (type == BlockType::OShape)
        return;
    for (auto& offset : cube_offsets) {
        offset.z = -offset.z;
        std::swap(offset.x, offset.z);
    }
}

void Block::RotateYCounterClockwise() {
    if (type == BlockType::OShape)
        return;
    for (auto& offset : cube_offsets) {
        offset.x = -offset.x;
        std::swap(offset.x, offset.z);
    }
}

void Block::RotateZClockwise() {
    if (type == BlockType::OShape)
        return;
    for (auto& offset : cube_offsets) {
        offset.x = -offset.x;
        std::swap(offset.x, offset.y);
    }
}

void Block::RotateZCounterClockwise() {
    if (type == BlockType::OShape)
        return;
    for (auto& offset : cube_offsets) {
        offset.y = -offset.y;
        std::swap(offset.x, offset.y);
    }
}

bool Block::TryTranslate(const Board3D& board, const glm::ivec3& value) {
    auto transformed_block = *this;
    transformed_block.Translate(value);
    if (transformed_block.IsValid(board)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateXClockwise(const Board3D& board) {
    auto transformed_block = *this;
    transformed_block.RotateXClockwise();
    if (transformed_block.IsValid(board)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateXCounterClockwise(const Board3D& board) {
    auto transformed_block = *this;
    transformed_block.RotateXCounterClockwise();
    if (transformed_block.IsValid(board)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateYClockwise(const Board3D& board) {
    auto transformed_block = *this;
    transformed_block.RotateYClockwise();
    if (transformed_block.IsValid(board)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateYCounterClockwise(const Board3D& board) {
    auto transformed_block = *this;
    transformed_block.RotateYCounterClockwise();
    if (transformed_block.IsValid(board)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateZClockwise(const Board3D& board) {
    auto transformed_block = *this;
    transformed_block.RotateZClockwise();
    if (transformed_block.IsValid(board)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateZCounterClockwise(const Board3D& board) {
    auto transformed_block = *this;
    transformed_block.RotateZCounterClockwise();
    if (transformed_block.IsValid(board)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateXClockwiseWithFix(const Board3D& board) {
    auto transformed_block = *this;
    transformed_block.RotateXClockwise();
    if (transformed_block.IsValid(board) ||
        transformed_block.TryFix(board, *this)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateXCounterClockwiseWithFix(const Board3D& board) {
    auto transformed_block = *this;
    transformed_block.RotateXCounterClockwise();
    if (transformed_block.IsValid(board) ||
        transformed_block.TryFix(board, *this)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateYClockwiseWithFix(const Board3D& board) {
    auto transformed_block = *this;
    transformed_block.RotateYClockwise();
    if (transformed_block.IsValid(board) ||
        transformed_block.TryFix(board, *this)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateYCounterClockwiseWithFix(const Board3D& board) {
    auto transformed_block = *this;
    transformed_block.RotateYCounterClockwise();
    if (transformed_block.IsValid(board) ||
        transformed_block.TryFix(board, *this)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateZClockwiseWithFix(const Board3D& board) {
    auto transformed_block = *this;
    transformed_block.RotateZClockwise();
    if (transformed_block.IsValid(board) ||
        transformed_block.TryFix(board, *this)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateZCounterClockwiseWithFix(const Board3D& board) {
    auto transformed_block = *this;
    transformed_block.RotateZCounterClockwise();
    if (transformed_block.IsValid(board) ||
        transformed_block.TryFix(board, *this)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::IsValid(const Board3D& board) const {
    for (auto& offset : cube_offsets) {
        auto world_pos = position + offset;
        if (!board.Contains(world_pos) || !board.IsEmpty(world_pos)) {
            return false;
        }
    }
    return true;
}

bool Block::IsCollidingWithOtherBlocks(const Board3D& board) const {
    for (auto& offset : cube_offsets) {
        auto world_pos = position + offset;
        if (board.Contains(world_pos) && !board.IsEmpty(world_pos)) {
            return true;
        }
    }
    return false;
}

bool Block::TryFix(const Board3D& board, const Block& prev_block) {
    assert(!IsValid(board));

    // NOTE(panmar): To prevent blocks 'teleportation' we consider only
    // horizontal translations toward previous valid block
    glm::ivec3 min_bounds, max_bounds, prev_min_bounds, prev_max_bounds;
    this->GetWorldBounds(min_bounds, max_bounds);
    prev_block.GetWorldBounds(prev_min_bounds, prev_max_bounds);
    // TODO(panmar): Use static container here
    std::vector<glm::ivec3> translations;

    // TODO(panmar): No sure about this implementations; it seems to be working
    // but more strict testing is needed here
    if (min_bounds.x < prev_min_bounds.x || max_bounds.x < prev_max_bounds.x) {
        translations.push_back(glm::ivec3(1, 0, 0));
    }
    if (min_bounds.x > prev_min_bounds.x || max_bounds.x > prev_max_bounds.x) {
        translations.push_back(glm::ivec3(-1, 0, 0));
    }

    if (min_bounds.z < prev_min_bounds.z || max_bounds.z < prev_max_bounds.z) {
        translations.push_back(glm::ivec3(0, 0, 1));
    }
    if (min_bounds.z > prev_min_bounds.z || max_bounds.z > prev_max_bounds.z) {
        translations.push_back(glm::ivec3(0, 0, -1));
    }

    // TODO(panmar): Use static container here
    std::vector<Block> candidates(translations.size(), *this);

    auto tries = 3;
    while (tries--) {
        for (size_t i = 0; i < translations.size(); ++i) {
            candidates[i].Translate(translations[i]);
            if (candidates[i].IsValid(board)) {
                *this = candidates[i];
                return true;
            }
        }
    }
    return false;
}

void Block::GetWorldBounds(glm::ivec3& min, glm::ivec3& max) const {
    min = glm::ivec3(INT_MAX, INT_MAX, INT_MAX);
    max = glm::ivec3(INT_MIN, INT_MIN, INT_MIN);
    for (auto& offset : cube_offsets) {
        auto world_pos = position + offset;
        min.x = std::min(min.x, world_pos.x);
        min.y = std::min(min.y, world_pos.y);
        min.z = std::min(min.z, world_pos.z);
        max.x = std::max(max.x, world_pos.x);
        max.y = std::max(max.y, world_pos.y);
        max.z = std::max(max.z, world_pos.z);
    }
}

void Update(GameState& state, f32 elapsed_seconds, const InputState& input,
            const glm::vec3& view_dir) {

    if (input.IsKeyPressed(Settings::key_pause)) {
        state.paused = !state.paused;
    }

    if (state.phase == GameState::Phase::Lost || state.paused) {
        return;
    }

    if (input.IsKeyPressed(Settings::key_block_horiz_rot_clock)) {
        if (view_dir.y < 0.f) {
            state.falling_block.TryRotateYClockwiseWithFix(state.board);
        } else {
            state.falling_block.TryRotateYCounterClockwiseWithFix(state.board);
        }
    }
    if (input.IsKeyPressed(Settings::key_block_horiz_rot_counterclock)) {
        if (view_dir.y < 0.f) {
            state.falling_block.TryRotateYCounterClockwiseWithFix(state.board);
        } else {
            state.falling_block.TryRotateYClockwiseWithFix(state.board);
        }
    }

    if (input.IsKeyPressed(Settings::key_block_vert_rot_away)) {
        auto view_dir_xz = view_dir;
        view_dir_xz.y = 0.f;
        view_dir_xz = glm::normalize(view_dir_xz);
        auto dot = glm::dot(glm::vec3(1.f, 0.f, 0.f), view_dir_xz);
        if (glm::abs(dot) < glm::cos(glm::pi<f32>() / 4.f)) {
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

    if (input.IsKeyPressed(Settings::key_block_vert_rot_towards)) {
        auto view_dir_xz = view_dir;
        view_dir_xz.y = 0.f;
        view_dir_xz = glm::normalize(view_dir_xz);
        auto dot = glm::dot(glm::vec3(1.f, 0.f, 0.f), view_dir_xz);
        if (glm::abs(dot) < glm::cos(glm::pi<f32>() / 4.f)) {
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

    if (input.IsKeyPressed(Settings::key_block_move_away)) {
        auto view_dir_xz = view_dir;
        view_dir_xz.y = 0.f;
        view_dir_xz = glm::normalize(view_dir_xz);
        auto dot = glm::dot(glm::vec3(1.f, 0.f, 0.f), view_dir_xz);
        if (glm::abs(dot) < glm::cos(glm::pi<f32>() / 4.f)) {
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

    if (input.IsKeyPressed(Settings::key_block_move_towards)) {
        auto view_dir_xz = view_dir;
        view_dir_xz.y = 0.f;
        view_dir_xz = glm::normalize(view_dir_xz);
        auto dot = glm::dot(glm::vec3(1.f, 0.f, 0.f), view_dir_xz);
        if (glm::abs(dot) < glm::cos(glm::pi<f32>() / 4.f)) {
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

    if (input.IsKeyDown(Settings::key_block_accelerate)) {
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

void SingleStep(GameState& state) {
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

bool CanFallingBlockFall(const GameState& state) {
    for (auto& cube_offset : state.falling_block.cube_offsets) {
        auto absolute_pos = state.falling_block.position + cube_offset;
        auto final_pos = absolute_pos - glm::ivec3(0, 1, 0);
        // TODO(panmar): For now, do not consider starting position
        if (final_pos.y >= 50)
            continue;
        if (final_pos.y < 0 || !state.board.IsEmpty(final_pos)) {
            return false;
        }
    }
    for (auto& cube_offset : state.falling_block.cube_offsets) {
        auto absolute_pos = state.falling_block.position + cube_offset;
        auto final_pos = absolute_pos - glm::ivec3(0, -1, 0);
        assert(final_pos.y >= 0);
    }

    return true;
}

void MergeFallingBlock(GameState& state) {
    for (auto& cube_offset : state.falling_block.cube_offsets) {
        auto absolute_pos = state.falling_block.position + cube_offset;
        state.board.Fill(absolute_pos, PackColor(state.falling_block.color));
    }
}

} // namespace GameLogic
