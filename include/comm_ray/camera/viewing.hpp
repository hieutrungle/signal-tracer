#pragma once

#ifndef VIEWING_HPP
#define VIEWING_HPP

#include "camera.hpp"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <memory>

namespace SignalTracer {
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
        Viewing(const Viewing& viewing);

        // Move constructor
        Viewing(Viewing&& viewing) noexcept;

        // Assignment operator
        Viewing& operator=(const Viewing& viewing);

        // Move assignment operator
        Viewing& operator=(Viewing&& viewing) noexcept;

        ~Viewing() = default;

        void reset();

        void process_mouse_button(int button, int action, int UTILS_UNUSED_PARAM(mods));
        void process_mouse_move(float xpos, float ypos);
        void process_keyboard(CameraMovement direction, float delta_time);
        void process_mouse_scroll(float yoffset);

        void pan(float xoffset, float yoffset, GLboolean constrain_pitch = true);
        void arcball_rotate(float xoffset, float yoffset);
        void zoom(float yoffset);

        void set_draw_mode(int draw_mode);
        void set_draw_reflection_mode(int draw_reflection_mode);
        void set_max_reflection_mode(int max_reflection_mode);
        void set_camera_fov(float fov);

        float get_delta_time() const;
        glm::mat4 get_view_matrix() const;
        float get_camera_fov() const;
        int get_draw_mode() const;
        int get_draw_reflection_mode() const;
        int get_maxreflection_mode() const;


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
}

#endif // !VIEWING_HPP