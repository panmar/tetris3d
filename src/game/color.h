#ifndef TETRIS3D_GAME_COLOR_H_
#define TETRIS3D_GAME_COLOR_H_

#include <cstdint>

namespace tetris3d {

struct Color {
    float r = 0.f;
    float g = 0.f;
    float b = 0.f;
    float a = 1.f;
};

struct ColorR8G8B8 {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

inline unsigned PackColor(const ColorR8G8B8& color) {
    return (color.r << 16) | (color.g << 8) | color.b;
}

Color FromPackedColorToColor(unsigned color_pack);

} // namespace tetris3d

#endif // TETRIS3D_GAME_COLOR_H_