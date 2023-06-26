#pragma once
#ifndef SCENE_CONFIG_HPP
#define SCENE_CONFIG_HPP

#include "config.hpp"
#include "utils.hpp"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "cyGL.h"

namespace SceneConfig {

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

    void default_window_hints() {
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    }

    void default_window_size(int& width, int& height) {
        width = 800;
        height = 600;
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

    void default_window_state(GLFWwindow* window) {
#ifdef FPS_VIEW
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#elif defined ORBIT_VIEW
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#endif
        return;
    }
}

#endif // !SCENE_CONFIG_HPP