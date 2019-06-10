#pragma once

#include <algorithm>
#include <random>

#include "common.h"
#include "glm/vec3.hpp"
#include "input.h"
#include "settings.h"

namespace GameLogic {

enum class BlockType {
    IShape,
    LShape,
    JShape,
    OShape,
    SShape,
    TShape,
    ZShape,

    // NOTE(panmar): This should be the last element
    Undefined
};

class Board3D;

class Block {
  public:
    static Block Create(BlockType type, const ColorR8G8B8& color,
                        const Board3D& board);
    static Block CreateRandom(const Board3D& board);

    void Translate(const glm::ivec3& value);
    void RotateXClockwise();
    void RotateXCounterClockwise();
    void RotateYClockwise();
    void RotateYCounterClockwise();
    void RotateZClockwise();
    void RotateZCounterClockwise();

    // NOTE(panmar): Before transforming a block check if there is enough
    // space for it; Returns true if succeeded, false otherwise.
    bool TryTranslate(const Board3D& board, const glm::ivec3& value);
    bool TryRotateXClockwise(const Board3D& board);
    bool TryRotateXCounterClockwise(const Board3D& board);
    bool TryRotateYClockwise(const Board3D& board);
    bool TryRotateYCounterClockwise(const Board3D& board);
    bool TryRotateZClockwise(const Board3D& board);
    bool TryRotateZCounterClockwise(const Board3D& board);

    // NOTE(panmar): Rotate a block and if it is invalid (out of bounds or
    // colliding with other blocks present in the board) try to translate it
    // horizontally so it become valid again; Shortest fix distance is chosen if
    // more than one
    bool TryRotateXClockwiseWithFix(const Board3D& board);
    bool TryRotateXCounterClockwiseWithFix(const Board3D& board);
    bool TryRotateYClockwiseWithFix(const Board3D& board);
    bool TryRotateYCounterClockwiseWithFix(const Board3D& board);
    bool TryRotateZClockwiseWithFix(const Board3D& board);
    bool TryRotateZCounterClockwiseWithFix(const Board3D& board);

    // NOTE(panmar): Check if a block collides with other blocks stored
    // already in the board or lies ouside its boundary
    bool IsValid(const Board3D& board) const;
    bool IsCollidingWithOtherBlocks(const Board3D& board) const;

    // Attempt to fix invalid block, which was made invalid after rotation
    // NOTE(panmar): It is a question if such fix should be made, but I find the
    // feeling of performing such a fix more in the spirit of tetris 2D
    // NOTE(panmar): In 2D tetris problem does not occure if pivot points are
    // chosen like here: https://i.stack.imgur.com/JLRFu.png
    // Chosing pivots like this in 3D (cube center) would result in less natural
    // rotation feeling; so it is tradeoff
    // @prev_block previous valid state of the invalidated block
    bool TryFix(const Board3D& board, const Block& prev_block);

    // Gets <min, max> corners of bounding box containing block in world space
    void GetWorldBounds(glm::ivec3& min, glm::ivec3& max) const;

    BlockType type = BlockType::Undefined;

    // NOTE(panmar): a world position of the block
    glm::ivec3 position;

    // NOTE(panmar): cubes positions in object space
    // TODO(panmar): IMPORTANT!! This could be stored as static array
    std::vector<glm::ivec3> cube_offsets;
    ColorR8G8B8 color;
};

class Board3D {
  public:
    Board3D(u32 _width = Settings::map_width, u32 _depth = Settings::map_depth,
            u32 _height = Settings::map_height)
        : width(_width), depth(_depth), height(_height),
          cells(width * depth * height, 0) {}

    void Fill(const glm::ivec3& position, u32 value) {
        auto index = PositionToIndex(position);
        cells[index] = value;
    }

    bool IsEmpty(const glm::ivec3& position) const {
        auto index = PositionToIndex(position);
        return !cells[index];
    }

    bool Contains(const glm::ivec3& position) const {
        if (position.x < 0 || position.y < 0 || position.z < 0 ||
            position.x >= static_cast<i32>(width) ||
            position.y >= static_cast<i32>(height) ||
            position.z >= static_cast<i32>(depth)) {
            return false;
        }
        return true;
    }

    size_t PositionToIndex(const glm::vec3& world_pos) const {
        return (world_pos.x * width + world_pos.z) +
               (world_pos.y * width * depth);
    }

    // Return number of filled layers erased; if there are no filled layers
    // return zero
    u32 EraseFilledLayers() {
        auto filled_layers = 0U;
        for (size_t layer = 0; layer < height;) {
            if (IsLayerFilled(layer)) {
                EraseLayer(layer);
                ++filled_layers;
            } else {
                ++layer;
            }
        }
        return filled_layers;
    }

    void EraseLayer(u32 layer) {
        for (size_t i = 0; i < width; ++i) {
            for (size_t j = 0; j < depth; ++j) {
                auto index = (i * width + j) + (layer * width * depth);
                cells[index] = 0;
            }
        }

        {
            for (size_t layer1 = layer; layer1 < height - 1; ++layer1) {
                auto layer2 = layer1 + 1;
                for (size_t i = 0; i < width; ++i) {
                    for (size_t j = 0; j < depth; ++j) {
                        auto index1 =
                            (i * width + j) + (layer1 * width * depth);
                        auto index2 =
                            (i * width + j) + (layer2 * width * depth);
                        cells[index1] = cells[index2];
                        cells[index2] = 0;
                    }
                }
            }
        }
    }

    // 'layer' is height index of the board (counting from ground (0-based))
    bool IsLayerFilled(u32 layer) const {
        for (size_t i = 0; i < width; ++i) {
            for (size_t j = 0; j < depth; ++j) {
                auto index = (i * width + j) + (layer * width * depth);
                if (!cells[index]) {
                    return false;
                }
            }
        }
        return true;
    }

    const u32 width = 0;
    const u32 depth = 0;
    const u32 height = 0;

    // NOTE(panmar): If zero cell is empty, otherwise is taken
    // If it is taken value store indicates a color for this particular cell
    // Color is packed as 32-bit: 0x00RRGGBB, leaving 8 bit for each channel
    // ----
    // 0        - empty
    // non-zero - taken (packed color)
    // ----
    // TODO(panmar): Change this scheme; Maybe we could store autogenerated
    // id here and move chosing colors to renderer? In that way even pure
    // black color could be taken if needed and game colorscheme could be
    // changed easily
    std::vector<u32> cells;
};

// The heart of Tetris3d game
// NOTE(panmar) all game logic state SHOULD be stored here!
struct GameState {
    GameState(
        f32 block_init_fall_step_seconds =
            Settings::block_init_fall_step_seconds,
        f32 block_max_fall_step_seconds = Settings::block_max_fall_step_seconds,
        f32 block_speed_inc_multiplier = Settings::block_speed_inc_multiplier,
        f32 block_speed_inc_period_seconds =
            Settings::block_speed_inc_period_seconds)
        : block_current_speed(block_init_fall_step_seconds),
          block_current_normal_speed(block_init_fall_step_seconds),
          block_max_fall_step_seconds(block_max_fall_step_seconds),
          block_speed_inc_multiplier(block_speed_inc_multiplier),
          block_speed_inc_period_seconds(block_speed_inc_period_seconds),
          seconds_from_last_speed_inc(block_speed_inc_period_seconds),
          seconds_to_next_block_fall(block_init_fall_step_seconds) {}

    Board3D board;
    Block falling_block;

    // We track phases to indicate what's currently going on inside algorithm
    // (so we can render something when certain phase occurse or play music)
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
    f32 block_current_speed = 0.f;
    // current normal speed (not accelerated)
    f32 block_current_normal_speed = 0.f;
    const f32 block_max_fall_step_seconds = 0.f;
    const f32 block_speed_inc_multiplier = 0.f;
    const f32 block_speed_inc_period_seconds = 0.f;

    f32 total_time = 0.f;
    f32 seconds_from_last_speed_inc = 0.f;
    f32 seconds_to_next_block_fall = 0.f;
};

// Time-aware update
void Update(GameState& state, f32 elapsed_seconds, const InputState& input,
            const glm::vec3& view_dir);

// Time-agnostic update
void SingleStep(GameState& state);

bool IsFallingBlockOutOfBounds(const GameState& state);

bool CanFallingBlockFall(const GameState& state);

void MergeFallingBlock(GameState& state);

}; // namespace GameLogic
