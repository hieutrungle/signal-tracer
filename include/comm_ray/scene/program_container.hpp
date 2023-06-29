#pragma once

#ifndef PROGRAM_CONTAINER_HPP
#define PROGRAM_CONTAINER_HPP

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "cyGL.h"
#include "drawable.hpp"
#include "lighting.hpp"
#include <string>
#include <vector>
#include <memory>

namespace signal_tracer {

    struct WindowParams {
        int width{};
        int height{};
        std::string title{};
        bool debug_flag{ false };
    };

    struct ShaderProgram {
        // A thin wrapper for cy::GLSLProgram
        std::string name;
        cy::GLSLProgram program;
        std::vector<std::shared_ptr<Light>> lights;
        std::vector<glm::vec3> light_colors;
        std::vector<std::shared_ptr<Drawable>> drawables;
        std::vector<glm::mat4> drawable_model_mats;
        glm::vec3 view_mat{ 1.0f };
        glm::vec3 proj_mat{ 1.0f };
    };
}

#endif // !PROGRAM_CONTAINER_HPP