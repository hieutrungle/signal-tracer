#include "camera.hpp"

namespace SignalTracer {
    Camera::Camera(
        glm::vec3 position,
        glm::vec3 target,
        glm::vec3 world_up,
        float fov
    )
        : m_position{ position }
        , m_target{ target }
        , m_world_up{ glm::normalize(world_up) }
        , m_fov{ fov } {
        init_camera();
    }

    glm::mat4 Camera::get_view_matrix() const {
        return glm::lookAt(m_position, m_position + m_cam_front, m_cam_up);
    }

    float Camera::get_fov() const {
        return m_fov;
    }

    void Camera::set_fov(float fov) {
        if (fov < 1.0f) { fov = 1.0f; }
        if (fov > 45.0f) { fov = 45.0f; }
        m_fov = fov;
    }

    void Camera::reset() {
        m_fov = CameraDefaultParam::fov;
        m_position = CameraDefaultParam::position;
        m_target = CameraDefaultParam::target;
        m_world_up = glm::normalize(CameraDefaultParam::world_up);
        init_camera();
    }

    void Camera::process_keyboard(CameraMovement direction, float velocity) {
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

    void Camera::pan(float xoffset, float yoffset, GLboolean constrain_pitch) {
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
        glm::vec3 front{ Utils::spherical2cartesian(m_yaw, m_pitch) };
        update_camera_vectors(front);
        m_target = m_position + glm::length(m_position - m_target) * m_cam_front;
    }

    void Camera::arcball_rotate(float xoffset, float yoffset) {
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
        m_position = m_target - radius * Utils::spherical2cartesian(m_yaw, m_pitch);

        update_camera_vectors(m_target - m_position);
    }

    void Camera::zoom(float yoffset) {
        m_position += m_cam_front * yoffset;
    }

    void Camera::init_camera() {
        Utils::cartesian2spherical(m_position, m_target, m_yaw, m_pitch);
        glm::vec3 front{ Utils::spherical2cartesian(m_yaw, m_pitch) };
        update_camera_vectors(front);
    }

    // Update camera orientation using Euler Angles. Update m_cam_front, m_cam_right and m_cam_up Vectors with the updated yaw and pitch values.
    void Camera::update_camera_vectors(glm::vec3 front) {
        m_cam_front = glm::normalize(front);
        m_cam_right = glm::normalize(glm::cross(m_cam_front, m_world_up));
        m_cam_up = glm::normalize(glm::cross(m_cam_right, m_cam_front));
    }
}