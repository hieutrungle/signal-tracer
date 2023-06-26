#pragma once 
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "config.hpp"
#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>

// TODO: add camera base class
// TODO: add FPS camera
// TODO: add orbit camera

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
    inline glm::vec3 position{ 0.0f, 0.0f, 0.0f };
    inline glm::vec3 target{ 0.0f, 0.0f, 0.0f };
    inline glm::vec3 cam_front{ 0.0f, 0.0f, -1.0f };
    inline glm::vec3 world_up{ 0.0f, 1.0f, 0.0f };
    inline float yaw{ -90.0 };
    inline float pitch{ 0.0 };
    inline float speed{ 5 };
    inline float mouse_sensitivity{ 0.1 };
    inline float zoom{ 45.0 };
}


class Camera {
public:
    // constructor with vectors
    Camera() = default;

    Camera(
        glm::vec3 position = CameraDefaultParam::position,
        glm::vec3 world_up = CameraDefaultParam::world_up,
        float yaw = CameraDefaultParam::yaw,
        float pitch = CameraDefaultParam::pitch,
        float speed = CameraDefaultParam::speed,
        float mouse_sensitivity = CameraDefaultParam::mouse_sensitivity,
        float zoom = CameraDefaultParam::zoom
    )
        : m_position{ position }
        , m_world_up{ glm::normalize(world_up) }
        , m_yaw{ yaw }
        , m_pitch{ pitch }
        , m_move_speed{ speed }
        , m_mouse_sensitity{ mouse_sensitivity }
        , m_zoom{ zoom } {

        update_camera_vectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ,
        float lookatX, float lookatY, float lookatZ,
        float upX, float upY, float upZ,
        float yaw = CameraDefaultParam::yaw,
        float pitch = CameraDefaultParam::pitch,
        float speed = CameraDefaultParam::speed,
        float mouse_sensitivity = CameraDefaultParam::mouse_sensitivity,
        float zoom = CameraDefaultParam::zoom
    )
        : m_position{ glm::vec3(posX, posY, posZ) }
        , m_target{ glm::vec3(lookatX, lookatY, lookatZ) }
        , m_world_up{ glm::normalize(glm::vec3(upX, upY, upZ)) }
        , m_yaw{ yaw }
        , m_pitch{ pitch }
        , m_move_speed{ speed }
        , m_mouse_sensitity{ mouse_sensitivity }
        , m_zoom{ zoom } {

        update_camera_vectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 get_view_matrix() {
        // if (m_first_mouse) {
        //     m_first_mouse = false;
        //     return glm::lookAt(m_position, m_target, m_cam_up);
        // }
        // else {
        //     return glm::lookAt(m_position, m_position + m_cam_front, m_cam_up);
        // }
        return glm::lookAt(m_position, m_position + m_cam_front, m_cam_up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void process_keyboard(CameraMovement direction, float delta_time) {
        float velocity = m_move_speed * delta_time;
        if (direction == CameraMovement::FORWARD)
            m_position += m_cam_front * velocity;
        if (direction == CameraMovement::BACKWARD)
            m_position -= m_cam_front * velocity;
        if (direction == CameraMovement::LEFT)
            m_position -= m_cam_right * velocity;
        if (direction == CameraMovement::RIGHT)
            m_position += m_cam_right * velocity;
        if (direction == CameraMovement::UP)
            m_position += m_cam_up * velocity;
        if (direction == CameraMovement::DOWN)
            m_position -= m_cam_up * velocity;
    }

    /*
        ---------------------------
        FPS camera
        ---------------------------
    */
    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void process_mouse_view_fps(float xoffset, float yoffset, GLboolean constrain_pitch = true) {
        xoffset *= m_mouse_sensitity;
        yoffset *= m_mouse_sensitity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrain_pitch) {
            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
            if (m_pitch < -89.0f)
                m_pitch = -89.0f;
        }

        // update m_cam_front, m_cam_right and m_cam_up Vectors using the updated Euler angles
        update_camera_vectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void process_mouse_scroll_fps(float yoffset) {
        m_zoom -= (float) yoffset;
        if (m_zoom < 1.0f)
            m_zoom = 1.0f;
        if (m_zoom > 55.0f)
            m_zoom = 55.0f;
    }

    /*
        ---------------------------
        Orbit camera
        ---------------------------
    */
    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    // void process_mouse_left_drag_orbit(float xoffset, float yoffset, GLboolean constrain_pitch = true) {
    //     update_camera_vectors();
    // }

    // void process_mouse_right_drag_orbit(float xoffset, float yoffset, GLboolean constrain_pitch = true) {
    //     update_camera_vectors();
    // }

    [[nodiscard]] glm::vec3 get_position() const {
        return m_position;
    }

    [[nodiscard]] glm::vec3 get_front() const {
        return m_cam_front;
    }

    [[nodiscard]] float get_zoom() const {
        return m_zoom;
    }

    [[nodiscard]] float get_yaw() const {
        return m_yaw;
    }

    [[nodiscard]] float get_pitch() const {
        return m_pitch;
    }

    [[nodiscard]] float get_move_speed() const {
        return m_move_speed;
    }

    [[nodiscard]] float get_mouse_sensitivity() const {
        return m_mouse_sensitity;
    }

    void set_position(const glm::vec3& position) {
        m_position = position;
    }
    void set_front(const glm::vec3& front) {
        m_cam_front = front;
    }

    void set_zoom(float zoom) {
        m_zoom = zoom;
    }

    glm::vec3 spherical2cartesian(float yaw, float pitch, float radius) {
        glm::vec3 cartesian;
        cartesian.x = radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        cartesian.y = radius * sin(glm::radians(pitch));
        cartesian.z = radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        return cartesian;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void update_camera_vectors() {
        // calculate the new m_cam_front vector
        float radius = 1.0;
#ifdef FPS_VIEW
        radius = 1.0;
#elif defined(ORBIT_VIEW)
        radius = glm::length(m_target - m_position);
#endif
        glm::vec3 front = spherical2cartesian(m_yaw, m_pitch, radius);

        m_cam_front = glm::normalize(front);
        m_cam_right = glm::normalize(glm::cross(m_cam_front, m_world_up));
        m_cam_up = glm::normalize(glm::cross(m_cam_right, m_cam_front));

    }

private:
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
    float m_move_speed{};
    float m_mouse_sensitity{};
    float m_zoom{};
    // firstMove
    bool m_first_mouse{ true };
};
#endif // !CAMERA_HPP