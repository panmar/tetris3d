#include "game/color.h"

namespace tetris3d {

Color FromPackedColorToColor(unsigned color_pack) {
    Color color;
    color.r = ((color_pack & 0xff0000) >> 16) / 255.f;
    color.g = ((color_pack & 0xff00) >> 8) / 255.f;
    color.b = (color_pack & 0xff) / 255.f;
    return color;
}

} // namespace tetris3d