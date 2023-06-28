#pragma once

#ifndef VIEWING_HPP
#define VIEWING_HPP

#include "base_camera.hpp"
#include "config.hpp"
#include "utils.hpp"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <memory>

class Viewing {
public:
    Viewing(std::shared_ptr<BaseCamera> camera_ptr
    )
        : m_camera_ptr{ camera_ptr }
        , m_first_mouse{ true }
        , m_mouse_left_down{ false }
        , m_mouse_right_down{ false }
        , m_mouse_middle_down{ false } {}

    // Copy constructor
    Viewing(const Viewing& viewing)
        : m_camera_ptr{ viewing.m_camera_ptr }
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
        : m_camera_ptr{ std::move(viewing.m_camera_ptr) }
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
        m_camera_ptr = viewing.m_camera_ptr;
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
        m_camera_ptr = std::move(viewing.m_camera_ptr);
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
        m_camera_ptr->reset();
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
        m_camera_ptr->process_keyboard(direction, velocity);
    }

    void process_mouse_scroll(float yoffset) {
        zoom(yoffset);
    }

    void pan(float xoffset, float yoffset, GLboolean constrain_pitch = true) {
        xoffset = xoffset * m_mouse_sensitivity;
        yoffset = yoffset * m_mouse_sensitivity;
        m_camera_ptr->pan(xoffset, yoffset, constrain_pitch);
    }

    void arcball_rotate(float xoffset, float yoffset) {
        xoffset = xoffset * m_mouse_sensitivity;
        yoffset = yoffset * m_mouse_sensitivity;
        m_camera_ptr->arcball_rotate(xoffset, yoffset);
    }

    void zoom(float yoffset) {
        yoffset = yoffset * m_move_speed * m_delta_time;
        m_camera_ptr->zoom(yoffset);
    }

    void set_camera_ptr(std::shared_ptr<BaseCamera> camera_ptr) {
        m_camera_ptr = camera_ptr;
    }

    void set_first_mouse(bool first_mouse) {
        m_first_mouse = first_mouse;
    }

    void set_mouse_left_down(bool mouse_left_down) {
        m_mouse_left_down = mouse_left_down;
    }

    void set_mouse_right_down(bool mouse_right_down) {
        m_mouse_right_down = mouse_right_down;
    }

    void set_mouse_middle_down(bool mouse_middle_down) {
        m_mouse_middle_down = mouse_middle_down;
    }

    void set_mouse_sensitivity(float mouse_sensitivity) {
        m_mouse_sensitivity = mouse_sensitivity;
    }

    void set_mouse_move_speed(float mouse_move_speed) {
        m_move_speed = mouse_move_speed;
    }

    void set_delta_time(float delta_time) {
        m_delta_time = delta_time;
    }

    void set_draw_mode(int draw_mode) {
        m_draw_mode = draw_mode;
    }

    void set_draw_reflection_mode(int draw_reflection_mode) {
        m_draw_reflection_mode = draw_reflection_mode;
    }

    void set_camera_fov(float fov) {
        m_camera_ptr->set_fov(fov);
    }

    [[nodiscard]] std::shared_ptr<BaseCamera> get_camera_ptr() const {
        return m_camera_ptr;
    }

    [[nodiscard]] bool get_first_mouse() const {
        return m_first_mouse;
    }

    [[nodiscard]] bool get_mouse_left_down() const {
        return m_mouse_left_down;
    }

    [[nodiscard]] bool get_mouse_right_down() const {
        return m_mouse_right_down;
    }

    [[nodiscard]] bool get_mouse_middle_down() const {
        return m_mouse_middle_down;
    }

    [[nodiscard]] float get_delta_time() const {
        return m_delta_time;
    }

    [[nodiscard]] glm::mat4 get_view_matrix() const {
        return m_camera_ptr->get_view_matrix();
    }

    [[nodiscard]] float get_camera_fov() const {
        return m_camera_ptr->get_fov();
    }

    [[nodiscard]] glm::vec3 get_position() const {
        return m_camera_ptr->get_position();
    }

    [[nodiscard]] glm::vec3 get_front() const {
        return m_camera_ptr->get_front();
    }

    [[nodiscard]] glm::vec3 get_up() const {
        return m_camera_ptr->get_up();
    }

    [[nodiscard]] glm::vec3 get_right() const {
        return m_camera_ptr->get_right();
    }

    [[nodiscard]] float get_yaw() const {
        return m_camera_ptr->get_yaw();
    }

    [[nodiscard]] float get_pitch() const {
        return m_camera_ptr->get_pitch();
    }

    // [[nodiscard]] float get_roll() const {
    //     return m_camera_ptr->get_roll();
    // }

    [[nodiscard]] float get_mouse_sensitivity() const {
        return m_mouse_sensitivity;
    }

    [[nodiscard]] float get_move_speed() const {
        return m_move_speed;
    }

    [[nodiscard]] int get_draw_mode() const {
        return m_draw_mode;
    }

    [[nodiscard]] int get_draw_reflection_mode() const {
        return m_draw_reflection_mode;
    }


    friend std::ostream& operator<<(std::ostream& os, const Viewing& viewing) {
        os << "viewing info: " << std::endl;
        os << "camera info: " << std::endl;
        os << *viewing.m_camera_ptr << std::endl;
        os << "first_mouse: " << viewing.m_first_mouse << std::endl;
        os << "mouse_left_down: " << viewing.m_mouse_left_down << std::endl;
        os << "mouse_right_down: " << viewing.m_mouse_right_down << std::endl;
        os << "mouse_middle_down: " << viewing.m_mouse_middle_down << std::endl;
        return os;
    }

private:
    // camera ptr
    std::shared_ptr<BaseCamera> m_camera_ptr{};
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
};


#endif // !VIEWING_HPP