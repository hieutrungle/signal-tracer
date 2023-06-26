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


//=============================================================================
// GLFW CALLBACKS
//=============================================================================

extern Viewing viewing;

namespace GLFWCallback {

    void framebuffer_size_callback(GLFWwindow* UTILS_UNUSED_PARAM(window), int width, int height) {
        glViewport(0, 0, width, height);
    }


    void keyCallback(GLFWwindow* window, int key, int UTILS_UNUSED_PARAM(scancode), int action, int UTILS_UNUSED_PARAM(mode)) {
        // close window
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        else if (key == GLFW_KEY_M && action == GLFW_PRESS) {

            viewing.set_draw_mode((viewing.get_draw_mode() + 1) % 4);
            int draw_mode{ viewing.get_draw_mode() };
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
            viewing.set_draw_reflection_mode((viewing.get_draw_reflection_mode() + 1) % 3);
        }
    }

    void mouse_cursor_callback(GLFWwindow* UTILS_UNUSED_PARAM(window), double x, double y) {
        viewing.process_mouse_move(static_cast<float>(x), static_cast<float>(y));
    }

    void mouse_button_callback(GLFWwindow* UTILS_UNUSED_PARAM(window), int button, int action, int mods) {
        // remember mouse position
        viewing.process_mouse_button(button, action, mods);
    }

    void scroll_callback(GLFWwindow* UTILS_UNUSED_PARAM(window), double UTILS_UNUSED_PARAM(xoffset), double yoffset) {
        // viewing.process_mouse_scroll(static_cast<float>(yoffset));
        viewing.set_camera_fov(viewing.get_camera_fov() - static_cast<float>(yoffset));
    }

    /*
        ---------------------------
        Keyboard
        ---------------------------
    */
    void move_camera_keys(GLFWwindow* window, float delta_time) {
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            viewing.process_keyboard(CameraMovement::FORWARD, delta_time);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            viewing.process_keyboard(CameraMovement::BACKWARD, delta_time);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            viewing.process_keyboard(CameraMovement::LEFT, delta_time);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            viewing.process_keyboard(CameraMovement::RIGHT, delta_time);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            viewing.process_keyboard(CameraMovement::UP, delta_time);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            viewing.process_keyboard(CameraMovement::DOWN, delta_time);
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
            viewing.reset();

    }
}



#endif // !GLFW_CALLBACK_HPP