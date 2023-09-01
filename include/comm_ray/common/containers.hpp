#pragma once

#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include "glm/glm.hpp"

namespace SignalTracer {
    struct Cell {
        glm::vec3 point{};
        float strength{ 0.0f };
        glm::vec3 color{ 0.0f };
    };

    struct TracerCell {
        glm::vec3 center;
        std::vector<float> strengths;
        std::vector<float> weights;
    };
}

#endif // !CONTAINER_HPP