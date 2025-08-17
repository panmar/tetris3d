#include "game/board.h"

namespace tetris3d {

Board::Board(int width, int depth, int height)
    : width_(width), depth_(depth), height_(height),
      cells_(width_ * depth_ * height_, 0) {}

void Board::Fill(const glm::ivec3& position, unsigned value) {
    auto index = PositionToIndex(position);
    cells_[index] = value;
}

bool Board::IsEmpty(const glm::ivec3& position) const {
    auto index = PositionToIndex(position);
    return !cells_[index];
}

bool Board::Contains(const glm::ivec3& position) const {
    if (position.x < 0 || position.y < 0 || position.z < 0 ||
        position.x >= static_cast<int32_t>(width_) ||
        position.y >= static_cast<int32_t>(height_) ||
        position.z >= static_cast<int32_t>(depth_)) {
        return false;
    }
    return true;
}

size_t Board::PositionToIndex(const glm::vec3& world_pos) const {
    return (world_pos.x * width_ + world_pos.z) +
           (world_pos.y * width_ * depth_);
}

// Return number of filled layers erased; if there are no filled layers
// return zero
unsigned Board::EraseFilledLayers() {
    auto filled_layers = 0U;
    for (size_t layer = 0; layer < height_;) {
        if (IsLayerFilled(layer)) {
            EraseLayer(layer);
            ++filled_layers;
        } else {
            ++layer;
        }
    }
    return filled_layers;
}

void Board::EraseLayer(unsigned layer) {
    for (size_t i = 0; i < width_; ++i) {
        for (size_t j = 0; j < depth_; ++j) {
            auto index = (i * width_ + j) + (layer * width_ * depth_);
            cells_[index] = 0;
        }
    }

    {
        for (size_t layer1 = layer; layer1 < height_ - 1; ++layer1) {
            auto layer2 = layer1 + 1;
            for (size_t i = 0; i < width_; ++i) {
                for (size_t j = 0; j < depth_; ++j) {
                    auto index1 = (i * width_ + j) + (layer1 * width_ * depth_);
                    auto index2 = (i * width_ + j) + (layer2 * width_ * depth_);
                    cells_[index1] = cells_[index2];
                    cells_[index2] = 0;
                }
            }
        }
    }
}

// 'layer' is height index of the board (counting from ground (0-based))
bool Board::IsLayerFilled(unsigned layer) const {
    for (size_t i = 0; i < width_; ++i) {
        for (size_t j = 0; j < depth_; ++j) {
            auto index = (i * width_ + j) + (layer * width_ * depth_);
            if (!cells_[index]) {
                return false;
            }
        }
    }
    return true;
}

} // namespace tetris3d