#include "config.h"

#include <format>

#include "error.h"

namespace tetris3d {

void ValidateConfig(const Config& config) {
    if (config.window_title.empty()) {
        throw Error("Window title is empty");
    }

    if (config.window_title.length() > Config::kMaximumWindowTitleLength) {
        throw Error(std::format("Window title exceeded maximum length ({}): {}",
                                Config::kMaximumWindowTitleLength,
                                config.window_title.length()));
    }
}

} // namespace tetris3d
