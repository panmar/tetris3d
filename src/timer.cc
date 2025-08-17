#include "timer.h"

namespace tetris3d {

void Timer::Update() {
    auto tick = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> diff = tick - prev_tick_;
    elapsed_seconds_ = diff.count();
    prev_tick_ = tick;
}

} // namespace tetris3d