#include "camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_normalized_axis.hpp"

namespace tetris3d {

Camera::Camera(const glm::vec3& position, const glm::vec3& target,
               const glm::vec3& up)
    : position_(position), target_(target), up_(up) {}

glm::vec3 Camera::GetForward() const {
    return glm::normalize(target_ - position_);
}

glm::vec3 Camera::GetLeft() const {
    return glm::normalize(glm::cross(GetUp(), GetForward()));
}

const glm::mat4& Camera::GetView() const {
    if (!is_view_valid_) {
        view_ = glm::lookAt(position_, target_, up_);
        is_view_valid_ = true;
    }
    return view_;
}

void Camera::SetPosition(const glm::vec3& value) {
    position_ = value;
    is_view_valid_ = false;
}

void Camera::SetTarget(const glm::vec3& value) {
    target_ = value;
    is_view_valid_ = false;
}

void Camera::SetUp(const glm::vec3& value) {
    up_ = value;
    is_view_valid_ = false;
}

void Camera::RotateAroundTarget(float left_rot, float up_rot) {
    glm::mat4 rot(1.f);
    rot = glm::rotateNormalizedAxis(rot, -left_rot, GetLeft());
    rot = glm::rotateNormalizedAxis(rot, -up_rot, glm::vec3(0.f, 1.f, 0.f));
    auto direction = target_ - position_;
    glm::vec3 new_direction = glm::vec4(direction, 0.f) * rot;
    glm::vec3 new_up = glm::vec4(up_, 0.f) * rot;

    if (glm::abs(glm::dot(glm::normalize(new_direction),
                          glm::vec3(0.f, 1.f, 0.f))) > 0.999f) {
        return;
    }

    SetPosition(target_ - new_direction);
    SetUp(new_up);
}

PerspectiveCamera::PerspectiveCamera(float aspect_ratio, float fov, float near,
                                     float far)
    : Camera(glm::vec3(0.f), glm::vec3(0.f, 0.0f, 1.f),
             glm::vec3(0.f, 1.0f, 0.f)),
      aspect_ratio_(aspect_ratio), fov_(fov), near_(near), far_(far) {}

void PerspectiveCamera::SetFov(float value) {
    fov_ = value;
    is_valid_projection_ = false;
}

void PerspectiveCamera::SetAspectRatio(float aspect) {
    aspect_ratio_ = aspect;
    is_valid_projection_ = false;
}

const glm::mat4& PerspectiveCamera::GetProjection() const {
    if (!is_valid_projection_) {
        projection_ = glm::perspective(fov_, aspect_ratio_, near_, far_);
        is_valid_projection_ = true;
    }
    return projection_;
}

} // namespace tetris3d