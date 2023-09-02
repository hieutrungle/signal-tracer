#include "callback.hpp"

namespace SignalTracer {
    GLFWCallback::GLFWCallback(GLFWwindow* window_ptr, std::shared_ptr<Viewing> viewing_ptr)
        : m_window_ptr(window_ptr)
        , m_viewing_ptr{ viewing_ptr } {}

    // Copy constructor
    GLFWCallback::GLFWCallback(const GLFWCallback& other)
        : m_window_ptr{ other.m_window_ptr }
        , m_viewing_ptr{ other.m_viewing_ptr } {}

    // Move constructor
    GLFWCallback::GLFWCallback(GLFWCallback&& other) noexcept
        : m_window_ptr{ other.m_window_ptr }
        , m_viewing_ptr{ other.m_viewing_ptr } {}

    // Copy assignment operator
    GLFWCallback& GLFWCallback::operator=(const GLFWCallback& other) {
        if (this != &other) {
            m_window_ptr = other.m_window_ptr;
            m_viewing_ptr = other.m_viewing_ptr;
        }
        return *this;
    }

    // Move assignment operator
    GLFWCallback& GLFWCallback::operator=(GLFWCallback&& other) noexcept {
        if (this != &other) {
            m_window_ptr = other.m_window_ptr;
            m_viewing_ptr = other.m_viewing_ptr;
        }
        return *this;
    }

    GLFWCallback::~GLFWCallback() {
        std::clog << "GLFWCallback destructor called." << std::endl;
        glfwSetFramebufferSizeCallback(m_window_ptr, nullptr);
        glfwSetKeyCallback(m_window_ptr, nullptr);
        glfwSetCursorPosCallback(m_window_ptr, nullptr);
        glfwSetMouseButtonCallback(m_window_ptr, nullptr);
        glfwSetScrollCallback(m_window_ptr, nullptr);
        glfwDestroyWindow(m_window_ptr);
    };

    void GLFWCallback::set_callback() {
        glfwSetWindowUserPointer(m_window_ptr, this);
        glfwSetFramebufferSizeCallback(m_window_ptr, framebuffer_size_callback);
        glfwSetKeyCallback(m_window_ptr, keyCallback);
        glfwSetCursorPosCallback(m_window_ptr, mouse_cursor_callback);
        glfwSetMouseButtonCallback(m_window_ptr, mouse_button_callback);
        glfwSetScrollCallback(m_window_ptr, scroll_callback);
    }

    void GLFWCallback::framebuffer_size_callback(int width, int height) {
        glViewport(0, 0, width, height);
    }

    void GLFWCallback::keyCallback(int key, int UTILS_UNUSED_PARAM(scancode), int action, int UTILS_UNUSED_PARAM(mode)) {
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
            m_viewing_ptr->set_draw_reflection_mode((m_viewing_ptr->get_draw_reflection_mode() + 1) % 22);
            std::clog << "draw_reflection_mode: " << m_viewing_ptr->get_draw_reflection_mode() << std::endl;
        }
        else if (key == GLFW_KEY_B && action == GLFW_PRESS) {
            m_viewing_ptr->set_draw_reflection_mode(0);
            std::clog << "draw_reflection_mode: " << m_viewing_ptr->get_draw_reflection_mode() << std::endl;
        }
        else if (key == GLFW_KEY_H && action == GLFW_PRESS)
            m_viewing_ptr->reset();
    }

    void GLFWCallback::mouse_cursor_callback(double x, double y) {
        m_viewing_ptr->process_mouse_move(static_cast<float>(x), static_cast<float>(y));
    }

    void GLFWCallback::mouse_button_callback(int button, int action, int mods) {
        // remember mouse position
        m_viewing_ptr->process_mouse_button(button, action, mods);
    }

    void GLFWCallback::scroll_callback(double UTILS_UNUSED_PARAM(xoffset), double yoffset) {
        m_viewing_ptr->set_camera_fov(m_viewing_ptr->get_camera_fov() - static_cast<float>(yoffset));
    }

}