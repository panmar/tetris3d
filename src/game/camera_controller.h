#ifndef TETRIS3D_GAME_CAMERA_CONTROLLER_H_
#define TETRIS3D_GAME_CAMERA_CONTROLLER_H_

#include "game/camera.h"
#include "input.h"
#include "config.h"

namespace tetris3d {

class OrbitCameraController {
public:
    OrbitCameraController(Config& config);

    void Update(PerspectiveCamera* camera, const Input& input);

private:
    Config& config_;
};

} // namespace tetris3d

#endif // TETRIS3D_GAME_CAMERA_CONTROLLER_H_