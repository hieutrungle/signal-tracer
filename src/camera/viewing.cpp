#include "viewing.hpp"

namespace SignalTracer {

    // Copy constructor
    Viewing::Viewing(const Viewing& viewing)
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
    Viewing::Viewing(Viewing&& viewing) noexcept
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
    Viewing& Viewing::operator=(const Viewing& viewing) {
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
    Viewing& Viewing::operator=(Viewing&& viewing) noexcept {
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

    void Viewing::reset() {
        m_camera.reset();
        m_first_mouse = true;
        m_mouse_left_down = false;
        m_mouse_right_down = false;
        m_mouse_middle_down = false;
    }

    void Viewing::process_mouse_button(int button, int action, int UTILS_UNUSED_PARAM(mods)) {
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

    void Viewing::process_mouse_move(float xpos, float ypos) {
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

    void Viewing::process_keyboard(CameraMovement direction, float delta_time) {
        float velocity = m_move_speed * delta_time;
        m_camera.process_keyboard(direction, velocity);
    }

    void Viewing::process_mouse_scroll(float yoffset) {
        zoom(yoffset);
    }

    void Viewing::pan(float xoffset, float yoffset, GLboolean constrain_pitch) {
        xoffset = xoffset * m_mouse_sensitivity;
        yoffset = yoffset * m_mouse_sensitivity;
        m_camera.pan(xoffset, yoffset, constrain_pitch);
    }

    void Viewing::arcball_rotate(float xoffset, float yoffset) {
        xoffset = xoffset * m_mouse_sensitivity;
        yoffset = yoffset * m_mouse_sensitivity;
        m_camera.arcball_rotate(xoffset, yoffset);
    }

    void Viewing::zoom(float yoffset) {
        yoffset = yoffset * m_move_speed * m_delta_time;
        m_camera.zoom(yoffset);
    }

    void Viewing::set_draw_mode(int draw_mode) {
        m_draw_mode = draw_mode;
    }

    void Viewing::set_draw_reflection_mode(int draw_reflection_mode) {
        m_draw_reflection_mode = draw_reflection_mode;
    }

    void Viewing::set_max_reflection_mode(int max_reflection_mode) {
        m_max_reflection_mode = max_reflection_mode;
    }

    void Viewing::set_camera_fov(float fov) {
        m_camera.set_fov(fov);
    }

    float Viewing::get_delta_time() const {
        return m_delta_time;
    }

    glm::mat4 Viewing::get_view_matrix() const {
        return m_camera.get_view_matrix();
    }

    float Viewing::get_camera_fov() const {
        return m_camera.get_fov();
    }

    int Viewing::get_draw_mode() const {
        return m_draw_mode;
    }

    int Viewing::get_draw_reflection_mode() const {
        return m_draw_reflection_mode;
    }

    int Viewing::get_maxreflection_mode() const {
        return m_max_reflection_mode;
    }
}