#pragma once

#include "common.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_normalized_axis.hpp"
#include "input.h"
#include "settings.h"

class Camera {
  public:
    Camera() {}
    Camera(const glm::vec3& position, const glm::vec3& target,
           const glm::vec3& up)
        : position(position), target(target), up(up) {}
    virtual ~Camera() = default;

    glm::vec3 GetPosition() const { return position; }

    glm::vec3 GetTarget() const { return target; }

    glm::vec3 GetUp() const { return up; }

    glm::vec3 GetDown() const { return -GetUp(); }

    glm::vec3 GetForward() const { return glm::normalize(target - position); }

    glm::vec3 GetBackward() const { return -GetForward(); }

    glm::vec3 GetLeft() const {
        return glm::normalize(glm::cross(GetUp(), GetForward()));
    }

    glm::vec3 GetRight() const { return -GetLeft(); }

    void SetPosition(const glm::vec3& value) {
        position = value;
        is_view_valid = false;
    }

    void SetTarget(const glm::vec3& value) {
        target = value;
        is_view_valid = false;
    }

    void SetUp(const glm::vec3& value) {
        up = value;
        is_view_valid = false;
    }

    const glm::mat4& GetView() const {
        if (!is_view_valid) {
            view = glm::lookAt(position, target, up);
            is_view_valid = true;
        }
        return view;
    }

    virtual const glm::mat4& GetProjection() const = 0;

    void RotateAroundTarget(f32 left_rot, f32 up_rot) {
        glm::mat4 rot(1.f);
        rot = glm::rotateNormalizedAxis(rot, -left_rot, GetLeft());
        rot = glm::rotateNormalizedAxis(rot, -up_rot, glm::vec3(0.f, 1.f, 0.f));
        auto direction = target - position;
        glm::vec3 new_direction = glm::vec4(direction, 0.f) * rot;
        glm::vec3 new_up = glm::vec4(up, 0.f) * rot;
        SetPosition(target - new_direction);
        SetUp(new_up);
    }

  private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    mutable glm::mat4 view;
    mutable bool is_view_valid = false;
};

class PerspectiveCamera : public Camera {
  public:
    PerspectiveCamera() {}
    PerspectiveCamera(f32 aspect_ratio, f32 fov, f32 near, f32 far)
        : aspect_ratio(aspect_ratio), fov(fov), near(near), far(far) {}
    virtual ~PerspectiveCamera() = default;

    const glm::mat4& GetProjection() const override {
        if (!is_valid_projection) {
            projection = glm::perspective(fov, aspect_ratio, near, far);
            is_valid_projection = true;
        }
        return projection;
    }

    f32 GetAspectRatio() const { return aspect_ratio; }
    f32 GetFov() const { return fov; }
    f32 GetNear() const { return near; }
    f32 GetFar() const { return far; }

    void SetFov(f32 value) {
        fov = value;
        is_valid_projection = false;
    }

  private:
    f32 aspect_ratio = 800.f / 600.f;
    f32 fov = glm::pi<f32>() / 2.f;
    f32 near = 0.1f;
    f32 far = 100.f;

    mutable glm::mat4 projection;
    mutable bool is_valid_projection = false;
};

class OrbitCameraController {
  public:
    // TODO(panmar): Change PerspectiveCamera to Camera base
    void Update(PerspectiveCamera* camera, const InputState& input) {
        assert(camera);
        auto left_rot = 0.f, up_rot = 0.f;
        auto left_rot_step = 0.1f, up_rot_step = 0.1f;
        if (input.IsKeyDown(Settings::key_playground_rotate_left)) {
            left_rot += left_rot_step;
        }
        if (input.IsKeyDown(Settings::key_playground_rotate_right)) {
            left_rot -= left_rot_step;
        }
        if (input.IsKeyDown(Settings::key_playground_rotate_up)) {
            up_rot -= up_rot_step;
        }
        if (input.IsKeyDown(Settings::key_playground_rotate_down)) {
            up_rot += up_rot_step;
        }

        camera->RotateAroundTarget(up_rot, left_rot);

        if (input.IsKeyDown(Settings::key_playground_zoom_in)) {
            camera->SetFov(camera->GetFov() + glm::pi<f32>() / 100.f);
        }

        if (input.IsKeyDown(Settings::key_playground_zoom_out)) {
            camera->SetFov(camera->GetFov() - glm::pi<f32>() / 100.f);
        }

        // TODO(panmar): Remove when move to OpenGL 3+
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(glm::degrees(camera->GetFov()), camera->GetAspectRatio(),
                       camera->GetNear(), camera->GetFar());
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(camera->GetPosition().x, camera->GetPosition().y,
                  camera->GetPosition().z, camera->GetTarget().x,
                  camera->GetTarget().y, camera->GetTarget().z,
                  camera->GetUp().x, camera->GetUp().y, camera->GetUp().z);
    }
};
