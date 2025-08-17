#include "game/camera_controller.h"

namespace tetris3d {

OrbitCameraController::OrbitCameraController(Config& config)
    : config_(config) {}

void OrbitCameraController::Update(PerspectiveCamera* camera,
                                   const Input& input) {
    if (!camera) {
        return;
    }

    auto left_rot = 0.f, up_rot = 0.f;
    auto left_rot_step = 0.1f, up_rot_step = 0.1f;
    if (input.IsKeyDown(config_.key_camera_rotate_left)) {
        left_rot += left_rot_step;
    }
    if (input.IsKeyDown(config_.key_camera_rotate_right)) {
        left_rot -= left_rot_step;
    }
    if (input.IsKeyDown(config_.key_camera_rotate_up)) {
        up_rot -= up_rot_step;
    }
    if (input.IsKeyDown(config_.key_camera_rotate_down)) {
        up_rot += up_rot_step;
    }

    camera->RotateAroundTarget(up_rot, left_rot);

    auto zoom_scale_step = glm::pi<float>() / 100.f;
    if (input.IsKeyDown(config_.key_camera_zoom_in)) {
        auto fov = camera->GetFov();
        fov += zoom_scale_step;
        fov = std::min(fov, config_.camera_zoom_min_fov);
        camera->SetFov(fov);
    }

    if (input.IsKeyDown(config_.key_camera_zoom_out)) {
        auto fov = camera->GetFov();
        fov -= zoom_scale_step;
        fov = std::max(fov, config_.camera_zoom_min_fov);
        camera->SetFov(fov);
    }
}

} // namespace tetris3d