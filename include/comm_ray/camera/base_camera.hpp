#pragma once 
#ifndef BASE_CAMERA_HPP
#define BASE_CAMERA_HPP


#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <cmath>
#include <vector>
#include <iostream>


// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

namespace CameraDefaultParam {
    inline glm::vec3 position{ 0.0f, 40.0f, 30.0f };
    inline glm::vec3 target{ 0.0f, 0.0f, 0.0f };
    inline glm::vec3 world_up{ 0.0f, 1.0f, 0.0f };
    inline float fov{ 45.0 };
}


class BaseCamera {
public:

    BaseCamera(
        glm::vec3 position = CameraDefaultParam::position,
        glm::vec3 target = CameraDefaultParam::target,
        glm::vec3 world_up = CameraDefaultParam::world_up,
        float fov = CameraDefaultParam::fov
    )
        : m_position{ position }
        , m_target{ target }
        , m_world_up{ glm::normalize(world_up) }
        , m_fov{ fov } {}

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    virtual glm::mat4 get_view_matrix() const {
        return glm::lookAt(m_position, m_position + m_cam_front, m_cam_up);
    }

    [[nodiscard]] float get_fov() const {
        return m_fov;
    }

    void set_fov(float fov) {
        if (fov < 1.0f) { fov = 1.0f; }
        if (fov > 45.0f) { fov = 45.0f; }
        m_fov = fov;
    }

    friend std::ostream& operator<<(std::ostream& os, const BaseCamera& base_camera) {
        os << "camera info: " << std::endl;
        os << "position: " << glm::to_string(base_camera.m_position) << std::endl;
        os << "target: " << glm::to_string(base_camera.m_target) << std::endl;
        os << "radius: " << glm::length(base_camera.m_position - base_camera.m_target) << std::endl;
        os << "front: " << glm::to_string(base_camera.m_cam_front) << std::endl;
        os << "up: " << glm::to_string(base_camera.m_cam_up) << std::endl;
        os << "right: " << glm::to_string(base_camera.m_cam_right) << std::endl;
        os << "world_up: " << glm::to_string(base_camera.m_world_up) << std::endl;
        os << "yaw: " << base_camera.m_yaw << std::endl;
        os << "pitch: " << base_camera.m_pitch << std::endl;
        return os;
    }

    virtual void reset() = 0;
    virtual void process_keyboard(CameraMovement direction, float velocity) = 0;
    virtual void pan(float xoffset, float yoffset, GLboolean constrain_pitch = true) = 0;
    virtual void arcball_rotate(float xoffset, float yoffset) = 0;
    virtual void zoom(float yoffset) = 0;

protected:

    virtual void init_camera() = 0;
    virtual void update_camera_vectors(glm::vec3 front) = 0;
    virtual glm::vec3 spherical2cartesian(float yaw, float pitch) = 0;
    virtual void cartesian2spherical(const glm::vec3& position, const glm::vec3& target, float& yaw, float& pitch) = 0;

protected:
    // camera Attributes
    glm::vec3 m_position{};
    glm::vec3 m_target{};
    glm::vec3 m_cam_front{};
    glm::vec3 m_cam_up{};
    glm::vec3 m_cam_right{};
    glm::vec3 m_world_up{};
    // euler Angles
    float m_yaw{};
    float m_pitch{};
    // camera options
    float m_fov{}; // field of view in degrees
};
#endif // !BASE_CAMERA_HPP