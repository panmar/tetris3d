#include "game/block.h"

#include <climits>
#include <cstdint>

#include "glm/geometric.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_normalized_axis.hpp"
#include "glm/vec3.hpp"

#include "game/board.h"

namespace tetris3d {

Block Block::Create(BlockType type, const ColorR8G8B8& color,
                    const Board& board) {
    Block block;
    block.type_ = type;
    block.color_ = color;
    block.position_ = glm::ivec3(board.GetWidth() / 2, board.GetHeight(),
                                board.GetDepth() / 2);

    switch (type) {
    case BlockType::LShape:
        block.cube_offsets_.emplace_back(0, 0, 0);
        block.cube_offsets_.emplace_back(-1, 0, 0);
        block.cube_offsets_.emplace_back(1, 0, 0);
        block.cube_offsets_.emplace_back(1, 1, 0);
        // TODO(panmar): block height could be calculated
        block.position_ -= glm::ivec3(0, 2, 0);
        break;
    case BlockType::IShape:
        block.cube_offsets_.emplace_back(0, 0, 0);
        block.cube_offsets_.emplace_back(-1, 0, 0);
        block.cube_offsets_.emplace_back(-2, 0, 0);
        block.cube_offsets_.emplace_back(1, 0, 0);
        block.position_ -= glm::ivec3(0, 1, 0);
        break;
    case BlockType::JShape:
        block.cube_offsets_.emplace_back(0, 0, 0);
        block.cube_offsets_.emplace_back(-1, 0, 0);
        block.cube_offsets_.emplace_back(1, 0, 0);
        block.cube_offsets_.emplace_back(-1, 1, 0);
        block.position_ -= glm::ivec3(0, 2, 0);
        break;
    case BlockType::OShape:
        block.cube_offsets_.emplace_back(0, 0, 0);
        block.cube_offsets_.emplace_back(0, 1, 0);
        block.cube_offsets_.emplace_back(-1, 0, 0);
        block.cube_offsets_.emplace_back(-1, 1, 0);
        block.cube_offsets_.emplace_back(0, 0, 1);
        block.cube_offsets_.emplace_back(0, 1, 1);
        block.cube_offsets_.emplace_back(-1, 0, 1);
        block.cube_offsets_.emplace_back(-1, 1, 1);
        block.position_ -= glm::ivec3(0, 2, 0);
        break;
    case BlockType::SShape:
        block.cube_offsets_.emplace_back(0, 0, 0);
        block.cube_offsets_.emplace_back(-1, 0, 0);
        block.cube_offsets_.emplace_back(0, 1, 0);
        block.cube_offsets_.emplace_back(1, 1, 0);
        block.position_ -= glm::ivec3(0, 2, 0);
        break;
    case BlockType::TShape:
        block.cube_offsets_.emplace_back(0, 0, 0);
        block.cube_offsets_.emplace_back(-1, 0, 0);
        block.cube_offsets_.emplace_back(1, 0, 0);
        block.cube_offsets_.emplace_back(0, 1, 0);
        block.position_ -= glm::ivec3(0, 2, 0);
        break;
    case BlockType::ZShape:
        block.cube_offsets_.emplace_back(0, 0, 0);
        block.cube_offsets_.emplace_back(0, 1, 0);
        block.cube_offsets_.emplace_back(-1, 1, 0);
        block.cube_offsets_.emplace_back(1, 0, 0);
        block.position_ -= glm::ivec3(0, 2, 0);
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

Block Block::CreateRandom(const Board& board) {
    unsigned block_type_idx =
        rand() % static_cast<unsigned>(BlockType::Undefined);
    BlockType type = static_cast<BlockType>(block_type_idx);

    // NOTE(panmar): pure black color is excluded;
    // It would be packed to 0x00000000 and could not be distinquished from
    // empty cell in tetris playground;

    // TODO(panmar): Randomization should probablly be done in more "natural"
    // way; Like having a shuffled vector of predefined colors
    ColorR8G8B8 color{0, 0, 0};
    while (!color.r && !color.b && !color.g) {
        color = ColorR8G8B8{static_cast<uint8_t>(10 + rand() % 246),
                            static_cast<uint8_t>(10 + rand() % 246),
                            static_cast<uint8_t>(10 + rand() % 246)};
    }
    return Block::Create(type, color, board);
}

void Block::Translate(const glm::ivec3& value) { position_ += value; }

void Block::RotateXClockwise() {
    if (type_ == BlockType::OShape)
        return;
    for (auto& offset : cube_offsets_) {
        offset.y = -offset.y;
        std::swap(offset.y, offset.z);
    }
}

void Block::RotateXCounterClockwise() {
    if (type_ == BlockType::OShape)
        return;
    for (auto& offset : cube_offsets_) {
        offset.z = -offset.z;
        std::swap(offset.y, offset.z);
    }
}

void Block::RotateYClockwise() {
    if (type_ == BlockType::OShape)
        return;
    for (auto& offset : cube_offsets_) {
        offset.z = -offset.z;
        std::swap(offset.x, offset.z);
    }
}

void Block::RotateYCounterClockwise() {
    if (type_ == BlockType::OShape)
        return;
    for (auto& offset : cube_offsets_) {
        offset.x = -offset.x;
        std::swap(offset.x, offset.z);
    }
}

void Block::RotateZClockwise() {
    if (type_ == BlockType::OShape)
        return;
    for (auto& offset : cube_offsets_) {
        offset.x = -offset.x;
        std::swap(offset.x, offset.y);
    }
}

void Block::RotateZCounterClockwise() {
    if (type_ == BlockType::OShape)
        return;
    for (auto& offset : cube_offsets_) {
        offset.y = -offset.y;
        std::swap(offset.x, offset.y);
    }
}

bool Block::TryTranslate(const Board& board, const glm::ivec3& value) {
    auto transformed_block = *this;
    transformed_block.Translate(value);
    if (transformed_block.IsValid(board)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateXClockwise(const Board& board) {
    auto transformed_block = *this;
    transformed_block.RotateXClockwise();
    if (transformed_block.IsValid(board)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateXCounterClockwise(const Board& board) {
    auto transformed_block = *this;
    transformed_block.RotateXCounterClockwise();
    if (transformed_block.IsValid(board)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateYClockwise(const Board& board) {
    auto transformed_block = *this;
    transformed_block.RotateYClockwise();
    if (transformed_block.IsValid(board)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateYCounterClockwise(const Board& board) {
    auto transformed_block = *this;
    transformed_block.RotateYCounterClockwise();
    if (transformed_block.IsValid(board)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateZClockwise(const Board& board) {
    auto transformed_block = *this;
    transformed_block.RotateZClockwise();
    if (transformed_block.IsValid(board)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateZCounterClockwise(const Board& board) {
    auto transformed_block = *this;
    transformed_block.RotateZCounterClockwise();
    if (transformed_block.IsValid(board)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateXClockwiseWithFix(const Board& board) {
    auto transformed_block = *this;
    transformed_block.RotateXClockwise();
    if (transformed_block.IsValid(board) ||
        transformed_block.TryFix(board, *this)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateXCounterClockwiseWithFix(const Board& board) {
    auto transformed_block = *this;
    transformed_block.RotateXCounterClockwise();
    if (transformed_block.IsValid(board) ||
        transformed_block.TryFix(board, *this)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateYClockwiseWithFix(const Board& board) {
    auto transformed_block = *this;
    transformed_block.RotateYClockwise();
    if (transformed_block.IsValid(board) ||
        transformed_block.TryFix(board, *this)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateYCounterClockwiseWithFix(const Board& board) {
    auto transformed_block = *this;
    transformed_block.RotateYCounterClockwise();
    if (transformed_block.IsValid(board) ||
        transformed_block.TryFix(board, *this)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateZClockwiseWithFix(const Board& board) {
    auto transformed_block = *this;
    transformed_block.RotateZClockwise();
    if (transformed_block.IsValid(board) ||
        transformed_block.TryFix(board, *this)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::TryRotateZCounterClockwiseWithFix(const Board& board) {
    auto transformed_block = *this;
    transformed_block.RotateZCounterClockwise();
    if (transformed_block.IsValid(board) ||
        transformed_block.TryFix(board, *this)) {
        *this = transformed_block;
        return true;
    }
    return false;
}

bool Block::IsValid(const Board& board) const {
    for (auto& offset : cube_offsets_) {
        auto world_pos = position_ + offset;
        if (!board.Contains(world_pos) || !board.IsEmpty(world_pos)) {
            return false;
        }
    }
    return true;
}

bool Block::IsCollidingWithOtherBlocks(const Board& board) const {
    for (auto& offset : cube_offsets_) {
        auto world_pos = position_ + offset;
        if (board.Contains(world_pos) && !board.IsEmpty(world_pos)) {
            return true;
        }
    }
    return false;
}

bool Block::TryFix(const Board& board, const Block& prev_block) {
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
    for (auto& offset : cube_offsets_) {
        auto world_pos = position_ + offset;
        min.x = std::min(min.x, world_pos.x);
        min.y = std::min(min.y, world_pos.y);
        min.z = std::min(min.z, world_pos.z);
        max.x = std::max(max.x, world_pos.x);
        max.y = std::max(max.y, world_pos.y);
        max.z = std::max(max.z, world_pos.z);
    }
}

} // namespace tetris3d