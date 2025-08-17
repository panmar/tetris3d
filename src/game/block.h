#ifndef TETRIS3D_GAME_BLOCK_H_
#define TETRIS3D_GAME_BLOCK_H_

#include <vector>

#include "glm/vec3.hpp"

#include "game/color.h"

namespace tetris3d {

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

class Board;

class Block {
public:
    static Block Create(BlockType type, const ColorR8G8B8& color,
                        const Board& board);
    static Block CreateRandom(const Board& board);

    const glm::ivec3& GetPosition() const { return position_; }
    glm::ivec3& GetPosition() { return position_; }
    ColorR8G8B8 GetColor() const { return color_; }

    const std::vector<glm::ivec3>& GetCubeOffsets() const {
        return cube_offsets_;
    }

    // Gets <min, max> corners of bounding box containing block in world space
    void GetWorldBounds(glm::ivec3& min, glm::ivec3& max) const;

    void Translate(const glm::ivec3& value);
    void RotateXClockwise();
    void RotateXCounterClockwise();
    void RotateYClockwise();
    void RotateYCounterClockwise();
    void RotateZClockwise();
    void RotateZCounterClockwise();

    // NOTE(panmar): Before transforming a block check if there is enough
    // space for it; Returns true if succeeded, false otherwise.
    bool TryTranslate(const Board& board, const glm::ivec3& value);
    bool TryRotateXClockwise(const Board& board);
    bool TryRotateXCounterClockwise(const Board& board);
    bool TryRotateYClockwise(const Board& board);
    bool TryRotateYCounterClockwise(const Board& board);
    bool TryRotateZClockwise(const Board& board);
    bool TryRotateZCounterClockwise(const Board& board);

    // NOTE(panmar): Rotate a block and if it is invalid (out of bounds or
    // colliding with other blocks present in the board) try to translate it
    // horizontally so it become valid again; Shortest fix distance is chosen if
    // more than one
    bool TryRotateXClockwiseWithFix(const Board& board);
    bool TryRotateXCounterClockwiseWithFix(const Board& board);
    bool TryRotateYClockwiseWithFix(const Board& board);
    bool TryRotateYCounterClockwiseWithFix(const Board& board);
    bool TryRotateZClockwiseWithFix(const Board& board);
    bool TryRotateZCounterClockwiseWithFix(const Board& board);

    // NOTE(panmar): Check if a block collides with other blocks stored
    // already in the board or lies ouside its boundary
    bool IsValid(const Board& board) const;
    bool IsCollidingWithOtherBlocks(const Board& board) const;

    // Attempt to fix invalid block, which was made invalid after rotation
    // NOTE(panmar): It is a question if such fix should be made, but I find the
    // feeling of performing such a fix more in the spirit of tetris 2D
    // NOTE(panmar): In 2D tetris problem does not occure if pivot points are
    // chosen like here: https://i.stack.imgur.com/JLRFu.png
    // Chosing pivots like this in 3D (cube center) would result in less natural
    // rotation feeling; so it is tradeoff
    // @prev_block previous valid state of the invalidated block
    bool TryFix(const Board& board, const Block& prev_block);

private:
    BlockType type_ = BlockType::Undefined;

    // NOTE(panmar): a world position of the block
    glm::ivec3 position_;

    // NOTE(panmar): cubes positions in object space
    std::vector<glm::ivec3> cube_offsets_;
    ColorR8G8B8 color_;
};

} // namespace tetris3d

#endif // TETRIS3D_GAME_BLOCK_H_