#pragma once
#ifndef GLFW_POINTER_HPP
#define GLFW_POINTER_HPP

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include <memory>

namespace SignalTracer {
    struct DestroyglfwWin {
        void operator()(GLFWwindow* ptr) {
            glfwDestroyWindow(ptr);
        }
    };

    typedef std::unique_ptr<GLFWwindow, DestroyglfwWin> unique_GLFWwindow;
}


#endif // GLFW_POINTER_HPP