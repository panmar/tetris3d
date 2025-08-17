#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_normalized_axis.hpp"
#include "glad/glad.h"
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"
#include "input.h"

namespace tetris3d {

class Camera {
public:
    Camera() {}
    Camera(const glm::vec3& position, const glm::vec3& target,
           const glm::vec3& up);
    virtual ~Camera() = default;

    glm::vec3 GetPosition() const { return position_; }
    glm::vec3 GetTarget() const { return target_; }
    glm::vec3 GetUp() const { return up_; }
    glm::vec3 GetDown() const { return -GetUp(); }
    glm::vec3 GetForward() const;
    glm::vec3 GetBackward() const { return -GetForward(); }
    glm::vec3 GetLeft() const;
    glm::vec3 GetRight() const { return -GetLeft(); }
    const glm::mat4& GetView() const;

    void SetPosition(const glm::vec3& value);
    void SetTarget(const glm::vec3& value);
    void SetUp(const glm::vec3& value);
    virtual const glm::mat4& GetProjection() const = 0;
    void RotateAroundTarget(float left_rot, float up_rot);

private:
    glm::vec3 position_;
    glm::vec3 target_;
    glm::vec3 up_;

    mutable glm::mat4 view_;
    mutable bool is_view_valid_ = false;
};

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera() {}
    PerspectiveCamera(float aspect_ratio, float fov, float near, float far);
    virtual ~PerspectiveCamera() = default;

    const glm::mat4& GetProjection() const override;

    float GetAspectRatio() const { return aspect_ratio_; }
    float GetFov() const { return fov_; }
    float GetNear() const { return near_; }
    float GetFar() const { return far_; }

    void SetFov(float value);
    void SetAspectRatio(float aspect);

private:
    float aspect_ratio_ = 1.0f;
    float fov_ = 1.1623f;
    float near_ = 0.1f;
    float far_ = 100.f;

    mutable glm::mat4 projection_;
    mutable bool is_valid_projection_ = false;
};

} // namespace tetris3d
