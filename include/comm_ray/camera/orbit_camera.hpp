#pragma once 
#ifndef ORBIT_CAMERA_HPP
#define ORBIT_CAMERA_HPP

#include "base_camera.hpp"
#include "config.hpp"
#include <cmath>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


class OrbitCamera : public BaseCamera {
public:
    // constructor with vectors
    OrbitCamera() = default;

    OrbitCamera(
        glm::vec3 position = CameraDefaultParam::position,
        glm::vec3 target = CameraDefaultParam::target,
        glm::vec3 world_up = CameraDefaultParam::world_up,
        float zoom = CameraDefaultParam::zoom
    )
        : BaseCamera(position, target, world_up, zoom) {
        init_camera();
    }

    void reset() override {
        m_position = CameraDefaultParam::position;
        m_target = CameraDefaultParam::target;
        m_world_up = glm::normalize(CameraDefaultParam::world_up);
        init_camera();
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void process_keyboard(CameraMovement direction, float velocity) override {
        if (direction == CameraMovement::FORWARD) {
            glm::vec3 tmp_position = m_position + m_cam_front * velocity;
            if (glm::length(tmp_position - m_target) > 1.0f) {
                m_position += m_cam_front * velocity;
            }
        }
        if (direction == CameraMovement::BACKWARD)
            m_position -= m_cam_front * velocity;
        if (direction == CameraMovement::LEFT) {
            m_position -= m_cam_right * velocity;
            m_target -= m_cam_right * velocity;
        }
        if (direction == CameraMovement::RIGHT) {
            m_position += m_cam_right * velocity;
            m_target += m_cam_right * velocity;
        }
        if (direction == CameraMovement::UP) {
            m_position += m_cam_up * velocity;
            m_target += m_cam_up * velocity;
        }
        if (direction == CameraMovement::DOWN) {
            m_position -= m_cam_up * velocity;
            m_target -= m_cam_up * velocity;
        }
    }

    void pan(float xoffset, float yoffset, GLboolean constrain_pitch = true) override {
        // xoffset *= m_mouse_sensitivity;
        // yoffset *= m_mouse_sensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrain_pitch) {
            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
            if (m_pitch < -89.0f)
                m_pitch = -89.0f;
        }
        glm::vec3 front {spherical2cartesian(m_yaw, m_pitch)};
        update_camera_vectors(front);
        m_target = m_position + glm::length(m_position - m_target) * m_cam_front;
    }

    void arcball_rotate(float xoffset, float yoffset) override {
        // rotate the camera's position around the target
        m_yaw += xoffset;
        float tmp_pitch = m_pitch + yoffset;

        if (tmp_pitch <= 89.0f && tmp_pitch >= -89.0f) {
            m_pitch = tmp_pitch;
        }
        if (m_yaw > 360.0f) {
            m_yaw -= 360.0f;
        }
        else if (m_yaw < -360.0f) {
            m_yaw += 360.0f;
        }

        float radius = glm::length(m_position - m_target);
        m_position = m_target - radius * spherical2cartesian(m_yaw, m_pitch);

        update_camera_vectors(m_target - m_position);
    }

    void zoom(float yoffset) override {
        m_position += m_cam_front * yoffset;
    }

protected:

    // Initializes the camera's position, radius, yaw, pitch, front, up and right vectors. This also be used in reset function.
    void init_camera() override {
        cartesian2spherical(m_position, m_target, m_yaw, m_pitch);
        glm::vec3 front {spherical2cartesian(m_yaw, m_pitch)};
        update_camera_vectors(front);
    }

    // Update camera orientation using Euler Angles. Update m_cam_front, m_cam_right and m_cam_up Vectors with the updated yaw and pitch values.
    void update_camera_vectors(glm::vec3 front) override {
        // calculate the new m_cam_front vector
        // glm::vec3 front {spherical2cartesian(m_yaw, m_pitch)};

        m_cam_front = glm::normalize(front);
        m_cam_right = glm::normalize(glm::cross(m_cam_front, m_world_up));
        m_cam_up = glm::normalize(glm::cross(m_cam_right, m_cam_front));
    }

    glm::vec3 spherical2cartesian(float yaw, float pitch) override {
        glm::vec3 cartesian;
        cartesian.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        cartesian.y = sin(glm::radians(pitch));
        cartesian.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        return cartesian;
    }

    void cartesian2spherical(const glm::vec3& position, const glm::vec3& target, float& yaw, float& pitch) override {
        // Convert from the position point and the target point to spherical coordinates
        glm::vec3 cartesian = position - target;
        yaw = -atan2(cartesian.z, cartesian.x) * 180 / Constant::PI;
        pitch = -atan2(cartesian.y, sqrt(cartesian.x * cartesian.x + cartesian.z * cartesian.z)) * 180 / Constant::PI;
    }

};

#endif // !ORBIT_CAMERA_HPP