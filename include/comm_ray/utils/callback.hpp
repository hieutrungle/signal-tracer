#pragma once

#ifndef GLFW_CALLBACK_HPP
#define GLFW_CALLBACK_HPP

#include "config.hpp"
#include "utils.hpp"
#include "viewing.hpp"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include <iostream>
#include <memory>


//=============================================================================
// GLFW CALLBACKS
//=============================================================================

// extern Viewing viewing;
// extern std::shared_ptr<Viewing> viewing_ptr;
// namespace GLFWCallback {

//     void framebuffer_size_callback(GLFWwindow* UTILS_UNUSED_PARAM(window), int width, int height) {
//         glViewport(0, 0, width, height);
//     }

//     void keyCallback(GLFWwindow* window, int key, int UTILS_UNUSED_PARAM(scancode), int action, int UTILS_UNUSED_PARAM(mode)) {
//         // close window
//         if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//             glfwSetWindowShouldClose(window, GL_TRUE);

//         else if (key == GLFW_KEY_M && action == GLFW_PRESS) {

//             viewing_ptr->set_draw_mode((viewing_ptr->get_draw_mode() + 1) % 4);
//             int draw_mode{ viewing_ptr->get_draw_mode() };
//             if (draw_mode == 0)        // no cull mode
//             {
//                 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//                 glEnable(GL_DEPTH_TEST);
//                 glDisable(GL_CULL_FACE);
//             }
//             else if (draw_mode == 1)  // fill mode
//             {
//                 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//                 glEnable(GL_DEPTH_TEST);
//                 glEnable(GL_CULL_FACE);
//             }
//             else if (draw_mode == 2)  // wireframe mode
//             {
//                 glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//                 glDisable(GL_DEPTH_TEST);
//                 glDisable(GL_CULL_FACE);
//             }
//             else                    // point mode
//             {
//                 glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
//                 glDisable(GL_DEPTH_TEST);
//                 glDisable(GL_CULL_FACE);
//             }
//         }
//         else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
//             viewing_ptr->set_draw_reflection_mode((viewing_ptr->get_draw_reflection_mode() + 1) % 3);
//         }
//     }

//     void mouse_cursor_callback(GLFWwindow* UTILS_UNUSED_PARAM(window), double x, double y) {
//         viewing_ptr->process_mouse_move(static_cast<float>(x), static_cast<float>(y));
//     }

//     void mouse_button_callback(GLFWwindow* UTILS_UNUSED_PARAM(window), int button, int action, int mods) {
//         // remember mouse position
//         viewing_ptr->process_mouse_button(button, action, mods);
//     }

//     void scroll_callback(GLFWwindow* UTILS_UNUSED_PARAM(window), double UTILS_UNUSED_PARAM(xoffset), double yoffset) {
//         // viewing_ptr->process_mouse_scroll(static_cast<float>(yoffset));
//         viewing_ptr->set_camera_fov(viewing_ptr->get_camera_fov() - static_cast<float>(yoffset));
//     }

//     /*
//         ---------------------------
//         Keyboard
//         ---------------------------
//     */
//     void move_camera_keys(GLFWwindow* window, float delta_time) {
//         if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
//             viewing_ptr->process_keyboard(CameraMovement::FORWARD, delta_time);
//         if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
//             viewing_ptr->process_keyboard(CameraMovement::BACKWARD, delta_time);
//         if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//             viewing_ptr->process_keyboard(CameraMovement::LEFT, delta_time);
//         if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//             viewing_ptr->process_keyboard(CameraMovement::RIGHT, delta_time);
//         if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//             viewing_ptr->process_keyboard(CameraMovement::UP, delta_time);
//         if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//             viewing_ptr->process_keyboard(CameraMovement::DOWN, delta_time);
//         if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
//             viewing_ptr->reset();
//     }
// }

namespace signal_tracer {
    class GLFWCallback {
    public:
        GLFWCallback() = default;
        GLFWCallback(GLFWwindow* window_ptr, std::shared_ptr<Viewing> viewing_ptr)
            : m_window_ptr(window_ptr)
            , m_viewing_ptr{ viewing_ptr } {}

        // Copy constructor
        GLFWCallback(const GLFWCallback& other)
            : m_window_ptr{ other.m_window_ptr }
            , m_viewing_ptr{ other.m_viewing_ptr } {}

        // Move constructor
        GLFWCallback(GLFWCallback&& other) noexcept
            : m_window_ptr{ other.m_window_ptr }
            , m_viewing_ptr{ other.m_viewing_ptr } {}

        // Copy assignment operator
        GLFWCallback& operator=(const GLFWCallback& other) {
            if (this != &other) {
                m_window_ptr = other.m_window_ptr;
                m_viewing_ptr = other.m_viewing_ptr;
            }
            return *this;
        }

        // Move assignment operator
        GLFWCallback& operator=(GLFWCallback&& other) noexcept {
            if (this != &other) {
                m_window_ptr = other.m_window_ptr;
                m_viewing_ptr = other.m_viewing_ptr;
            }
            return *this;
        }

        ~GLFWCallback() {
            std::clog << "GLFWCallback destructor called." << std::endl;
            glfwSetFramebufferSizeCallback(m_window_ptr, nullptr);
            glfwSetKeyCallback(m_window_ptr, nullptr);
            glfwSetCursorPosCallback(m_window_ptr, nullptr);
            glfwSetMouseButtonCallback(m_window_ptr, nullptr);
            glfwSetScrollCallback(m_window_ptr, nullptr);
            glfwDestroyWindow(m_window_ptr);
        };

        void set_callback() {
            glfwSetWindowUserPointer(m_window_ptr, this);
            glfwSetFramebufferSizeCallback(m_window_ptr, framebuffer_size_callback);
            glfwSetKeyCallback(m_window_ptr, keyCallback);
            glfwSetCursorPosCallback(m_window_ptr, mouse_cursor_callback);
            glfwSetMouseButtonCallback(m_window_ptr, mouse_button_callback);
            glfwSetScrollCallback(m_window_ptr, scroll_callback);
        }

        void framebuffer_size_callback(int width, int height) {
            glViewport(0, 0, width, height);
        }

        void keyCallback(int key, int UTILS_UNUSED_PARAM(scancode), int action, int UTILS_UNUSED_PARAM(mode)) {
            // close window
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                glfwSetWindowShouldClose(m_window_ptr, GL_TRUE);

            else if (key == GLFW_KEY_M && action == GLFW_PRESS) {

                m_viewing_ptr->set_draw_mode((m_viewing_ptr->get_draw_mode() + 1) % 4);
                int draw_mode{ m_viewing_ptr->get_draw_mode() };
                if (draw_mode == 0)        // no cull mode
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glEnable(GL_DEPTH_TEST);
                    glDisable(GL_CULL_FACE);
                }
                else if (draw_mode == 1)  // fill mode
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glEnable(GL_DEPTH_TEST);
                    glEnable(GL_CULL_FACE);
                }
                else if (draw_mode == 2)  // wireframe mode
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDisable(GL_DEPTH_TEST);
                    glDisable(GL_CULL_FACE);
                }
                else                    // point mode
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                    glDisable(GL_DEPTH_TEST);
                    glDisable(GL_CULL_FACE);
                }
            }
            else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
                m_viewing_ptr->set_draw_reflection_mode((m_viewing_ptr->get_draw_reflection_mode() + 1) % 3);
            }
            else if (key == GLFW_KEY_H && action == GLFW_PRESS)
                m_viewing_ptr->reset();
        }

        void mouse_cursor_callback(double x, double y) {
            m_viewing_ptr->process_mouse_move(static_cast<float>(x), static_cast<float>(y));
        }

        void mouse_button_callback(int button, int action, int mods) {
            // remember mouse position
            m_viewing_ptr->process_mouse_button(button, action, mods);
        }

        void scroll_callback(double UTILS_UNUSED_PARAM(xoffset), double yoffset) {
            m_viewing_ptr->set_camera_fov(m_viewing_ptr->get_camera_fov() - static_cast<float>(yoffset));
        }


        static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
            GLFWCallback* callback = static_cast<GLFWCallback*>(glfwGetWindowUserPointer(window));
            callback->framebuffer_size_callback(width, height);
        }


        /*
            ---------------------------
            Keyboard
            ---------------------------
        */
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
            GLFWCallback* callback = static_cast<GLFWCallback*>(glfwGetWindowUserPointer(window));
            callback->keyCallback(key, scancode, action, mode);
        }

        /*
            ---------------------------
            Mouse
            ---------------------------
        */
        static void mouse_cursor_callback(GLFWwindow* window, double x, double y) {
            GLFWCallback* callback = static_cast<GLFWCallback*>(glfwGetWindowUserPointer(window));
            callback->mouse_cursor_callback(x, y);
        }

        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
            GLFWCallback* callback = static_cast<GLFWCallback*>(glfwGetWindowUserPointer(window));
            callback->mouse_button_callback(button, action, mods);
        }

        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
            GLFWCallback* callback = static_cast<GLFWCallback*>(glfwGetWindowUserPointer(window));
            callback->scroll_callback(xoffset, yoffset);
        }

    private:
        GLFWwindow* m_window_ptr;
        std::shared_ptr<Viewing> m_viewing_ptr;
    };
}

#endif // !GLFW_CALLBACK_HPP