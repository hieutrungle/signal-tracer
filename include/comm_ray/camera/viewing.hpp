#pragma once

#ifndef VIEWING_HPP
#define VIEWING_HPP

#include "camera.hpp"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <memory>

class Viewing {
public:
    Viewing(Camera camera
    )
        : m_camera{ camera }
        , m_first_mouse{ true }
        , m_mouse_left_down{ false }
        , m_mouse_right_down{ false }
        , m_mouse_middle_down{ false } {}

    // Copy constructor
    Viewing(const Viewing& viewing)
        : m_camera{ viewing.m_camera }
        , m_first_mouse{ viewing.m_first_mouse }
        , m_mouse_left_down{ viewing.m_mouse_left_down }
        , m_mouse_right_down{ viewing.m_mouse_right_down }
        , m_mouse_middle_down{ viewing.m_mouse_middle_down }
        , m_mouse_sensitivity{ viewing.m_mouse_sensitivity }
        , m_move_speed{ viewing.m_move_speed }
        , m_delta_time{ viewing.m_delta_time }
        , m_draw_mode{ viewing.m_draw_mode }
        , m_draw_reflection_mode{ viewing.m_draw_reflection_mode } {}

    // Move constructor
    Viewing(Viewing&& viewing) noexcept
        : m_camera{ std::move(viewing.m_camera) }
        , m_first_mouse{ std::move(viewing.m_first_mouse) }
        , m_mouse_left_down{ std::move(viewing.m_mouse_left_down) }
        , m_mouse_right_down{ std::move(viewing.m_mouse_right_down) }
        , m_mouse_middle_down{ std::move(viewing.m_mouse_middle_down) }
        , m_mouse_sensitivity{ std::move(viewing.m_mouse_sensitivity) }
        , m_move_speed{ std::move(viewing.m_move_speed) }
        , m_delta_time{ std::move(viewing.m_delta_time) }
        , m_draw_mode{ std::move(viewing.m_draw_mode) }
        , m_draw_reflection_mode{ std::move(viewing.m_draw_reflection_mode) } {}

    // Assignment operator
    Viewing& operator=(const Viewing& viewing) {
        if (this == &viewing) {
            return *this;
        }
        m_camera = viewing.m_camera;
        m_first_mouse = viewing.m_first_mouse;
        m_mouse_left_down = viewing.m_mouse_left_down;
        m_mouse_right_down = viewing.m_mouse_right_down;
        m_mouse_middle_down = viewing.m_mouse_middle_down;
        m_mouse_sensitivity = viewing.m_mouse_sensitivity;
        m_move_speed = viewing.m_move_speed;
        m_delta_time = viewing.m_delta_time;
        m_draw_mode = viewing.m_draw_mode;
        m_draw_reflection_mode = viewing.m_draw_reflection_mode;
        return *this;
    }

    // Move assignment operator
    Viewing& operator=(Viewing&& viewing) noexcept {
        if (this == &viewing) {
            return *this;
        }
        m_camera = std::move(viewing.m_camera);
        m_first_mouse = std::move(viewing.m_first_mouse);
        m_mouse_left_down = std::move(viewing.m_mouse_left_down);
        m_mouse_right_down = std::move(viewing.m_mouse_right_down);
        m_mouse_middle_down = std::move(viewing.m_mouse_middle_down);
        m_mouse_sensitivity = std::move(viewing.m_mouse_sensitivity);
        m_move_speed = std::move(viewing.m_move_speed);
        m_delta_time = std::move(viewing.m_delta_time);
        m_draw_mode = std::move(viewing.m_draw_mode);
        m_draw_reflection_mode = std::move(viewing.m_draw_reflection_mode);
        return *this;
    }

    ~Viewing() = default;

    void reset() {
        m_camera.reset();
        m_first_mouse = true;
        m_mouse_left_down = false;
        m_mouse_right_down = false;
        m_mouse_middle_down = false;
    }

    void process_mouse_button(int button, int action, int UTILS_UNUSED_PARAM(mods)) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            m_mouse_left_down = true;
        }
        else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            m_mouse_left_down = false;
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            m_mouse_right_down = true;
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
            m_mouse_right_down = false;
        }
        else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
            m_mouse_middle_down = true;
        }
        else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
            m_mouse_middle_down = false;
        }
    }

    void process_mouse_move(float xpos, float ypos) {
        if (m_mouse_left_down || m_mouse_right_down || m_mouse_middle_down) {
            if (m_first_mouse) {
                m_last_x = xpos;
                m_last_y = ypos;
                m_first_mouse = false;
            }

            float xoffset = xpos - m_last_x;
            float yoffset = m_last_y - ypos; // reversed since y-coordinates go from bottom to top

            m_last_x = xpos;
            m_last_y = ypos;

            if (m_mouse_left_down) {
                pan(xoffset, yoffset);
            }
            else if (m_mouse_right_down) {
                arcball_rotate(xoffset, yoffset);
            }
            else if (m_mouse_middle_down) {
                zoom(yoffset);
            }
        }
        else {
            // not dragging
            m_first_mouse = true;
        }
    }

    void process_keyboard(CameraMovement direction, float delta_time) {
        float velocity = m_move_speed * delta_time;
        m_camera.process_keyboard(direction, velocity);
    }

    void process_mouse_scroll(float yoffset) {
        zoom(yoffset);
    }

    void pan(float xoffset, float yoffset, GLboolean constrain_pitch = true) {
        xoffset = xoffset * m_mouse_sensitivity;
        yoffset = yoffset * m_mouse_sensitivity;
        m_camera.pan(xoffset, yoffset, constrain_pitch);
    }

    void arcball_rotate(float xoffset, float yoffset) {
        xoffset = xoffset * m_mouse_sensitivity;
        yoffset = yoffset * m_mouse_sensitivity;
        m_camera.arcball_rotate(xoffset, yoffset);
    }

    void zoom(float yoffset) {
        yoffset = yoffset * m_move_speed * m_delta_time;
        m_camera.zoom(yoffset);
    }

    void set_draw_mode(int draw_mode) {
        m_draw_mode = draw_mode;
    }

    void set_draw_reflection_mode(int draw_reflection_mode) {
        m_draw_reflection_mode = draw_reflection_mode;
    }

    void set_max_reflection_mode(int max_reflection_mode) {
        m_max_reflection_mode = max_reflection_mode;
    }

    void set_camera_fov(float fov) {
        m_camera.set_fov(fov);
    }

    [[nodiscard]] float get_delta_time() const {
        return m_delta_time;
    }

    [[nodiscard]] glm::mat4 get_view_matrix() const {
        return m_camera.get_view_matrix();
    }

    [[nodiscard]] float get_camera_fov() const {
        return m_camera.get_fov();
    }

    [[nodiscard]] int get_draw_mode() const {
        return m_draw_mode;
    }

    [[nodiscard]] int get_draw_reflection_mode() const {
        return m_draw_reflection_mode;
    }

    [[nodiscard]] int get_maxreflection_mode() const {
        return m_max_reflection_mode;
    }


    friend std::ostream& operator<<(std::ostream& os, const Viewing& viewing) {
        os << "viewing info: " << std::endl;
        os << "camera info: " << std::endl;
        os << viewing.m_camera << std::endl;
        os << "first_mouse: " << viewing.m_first_mouse << std::endl;
        os << "mouse_left_down: " << viewing.m_mouse_left_down << std::endl;
        os << "mouse_right_down: " << viewing.m_mouse_right_down << std::endl;
        os << "mouse_middle_down: " << viewing.m_mouse_middle_down << std::endl;
        return os;
    }

private:
    // camera ptr
    Camera m_camera{};
    // movement
    float m_last_x{};
    float m_last_y{};
    // firstMove
    bool m_first_mouse{ true };
    // mouse click
    bool m_mouse_left_down{ false };
    bool m_mouse_right_down{ false };
    bool m_mouse_middle_down{ false };
    // mouse sensitivity
    float m_mouse_sensitivity{ 0.1f };
    float m_move_speed{ 20.0f };
    // delta_time
    float m_delta_time{};
    // draw mode
    int m_draw_mode{ 0 };
    // draw reflections
    int m_draw_reflection_mode{ 0 };
    int m_max_reflection_mode{ 0 };
};


#endif // !VIEWING_HPP