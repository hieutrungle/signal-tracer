#pragma once

#ifndef SCENE_HPP
#define SCENE_HPP

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "cyGL.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/glm.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <memory>

namespace signal_tracer {
    class Scene {
    public:
        Scene(int width, int height, const std::string& title = "CommRay")
            : m_width(width), m_height(height), m_title(title) {
            init();
        }

        ~Scene() {
            glfwTerminate();
        }

        void init() {
            if (!glfwInit()) {
                std::cout << "Failed to initialize GLFW" << std::endl;
                exit(EXIT_FAILURE);
            }

            default_window_hints();
            m_window = glfwCreateWindow(WIDTH, HEIGHT, "CommRay", nullptr, nullptr);
            if (!m_window) {
                std::cout << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                exit(EXIT_FAILURE);
            }
            glfwMakeContextCurrent(window);

            // Initialize GLAD
            // ----------------------------------
            int version = gladLoadGL(glfwGetProcAddress);
            if (version == 0) {
                std::cerr << "Failed to initialize GLAD\n";
                return -1;
            }
            std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << '\n';

            default_gl_state();
            default_window_state();
            set_debug();
            set_viewport(width, height);
            set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);

            // Calback functions
            glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        }

        void set_debug() {
            GLint flags;
            glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
            if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
                CY_GL_REGISTER_DEBUG_CALLBACK;
                puts("Debugging enabled.");
            }
            else
                puts("Debugging disabled.");
        }

        void default_gl_state() {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        void default_window_hints() {
            glfwWindowHint(GLFW_SAMPLES, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#ifdef DEBUG
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
        }

        void default_window_state() {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            return;
        }

        void set_viewport(int width, int height) {
            glViewport(0, 0, width, height);
        }

        void set_clear_color(float r, float g, float b, float a) {
            glClearColor(r, g, b, a);
        }

        void clear() {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void swap_buffers() {
            glfwSwapBuffers(window);
        }

        void poll_events() {
            glfwPollEvents();
        }

        bool should_close() {
            return glfwWindowShouldClose(window);
        }

        GLFWwindow* get_window() {
            return window;
        }

        int get_width() {
            return width;
        }

        int get_height() {
            return height;
        }

        float get_aspect_ratio() {
            return static_cast<float>(width) / static_cast<float>(height);
        }

        std::string get_title() {
            return title;
        }

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
            glViewport(0, 0, width, height);
        }



    private:
        GLFWwindow* m_window{};
        int m_width{};
        int m_height{};
        std::string m_title{"CommRay"};
    };
}


#endif // !SCENE_HPP