#ifndef TETRIS3D_TIMER_H_
#define TETRIS3D_TIMER_H_

#include <chrono>

namespace tetris3d {

class Timer {
public:
    float GetElapsedSeconds() const { return elapsed_seconds_; }
    void Update();

private:
    std::chrono::high_resolution_clock::time_point prev_tick_ =
        std::chrono::high_resolution_clock::now();
    float elapsed_seconds_ = 0.0f;
};

} // namespace tetris3d

#endif // TETRIS3D_TIMER_H_