#pragma once

#ifndef GLFW_CALLBACK_HPP
#define GLFW_CALLBACK_HPP

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

namespace SignalTracer {
    class GLFWCallback {
    public:
        GLFWCallback() = default;
        GLFWCallback(GLFWwindow* window_ptr, std::shared_ptr<Viewing> viewing_ptr);

        // Copy constructor
        GLFWCallback(const GLFWCallback& other);

        // Move constructor
        GLFWCallback(GLFWCallback&& other) noexcept;

        // Copy assignment operator
        GLFWCallback& operator=(const GLFWCallback& other);

        // Move assignment operator
        GLFWCallback& operator=(GLFWCallback&& other) noexcept;

        ~GLFWCallback();

        void set_callback();

        void framebuffer_size_callback(int width, int height);
        void keyCallback(int key, int UTILS_UNUSED_PARAM(scancode), int action, int UTILS_UNUSED_PARAM(mode));
        void mouse_cursor_callback(double x, double y);
        void mouse_button_callback(int button, int action, int mods);
        void scroll_callback(double UTILS_UNUSED_PARAM(xoffset), double yoffset);

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