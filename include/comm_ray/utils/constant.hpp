#pragma once

#ifndef CONSTANT_HPP
#define CONSTANT_HPP

#include <glm/glm.hpp>
#include <cmath>
#include <iostream>

namespace Constant {
    template<typename T>
    const T INFINITY_POS_T = std::numeric_limits<T>::infinity();
    template<typename T>
    const T INFINITY_NEG_T = -std::numeric_limits<T>::infinity();
    const double PI = std::numbers::pi;

    const float LIGHT_SPEED = 299792458.0;

    const float INFINITY_POS = INFINITY_POS_T<float>;
    const float INFINITY_NEG = INFINITY_NEG_T<float>;

    const float EPSILON = 1e-5;

    const inline glm::vec3 NEG_INF_VEC = glm::vec3(INFINITY_NEG);
    const inline glm::vec3 MIN_BOUND = glm::vec3(INFINITY_POS);
    const inline glm::vec3 MAX_BOUND = glm::vec3(INFINITY_NEG);

    /*
        ---------------------------------
        COLOR CONSTANTS
        ---------------------------------
    */
    const inline glm::vec3 RED = glm::vec3(1.0f, 0.0f, 0.0f);
    const inline glm::vec3 GREEN = glm::vec3(0.0f, 1.0f, 0.0f);
    const inline glm::vec3 BLUE = glm::vec3(0.0f, 0.0f, 1.0f);
    const inline glm::vec3 YELLOW = glm::vec3(1.0f, 1.0f, 0.0f);
    const inline glm::vec3 CYAN = glm::vec3(0.0f, 1.0f, 1.0f);
    const inline glm::vec3 MAGENTA = glm::vec3(1.0f, 0.0f, 1.0f);
    const inline glm::vec3 ORANGE = glm::vec3(1.0f, 0.5f, 0.0f);
    const inline glm::vec3 PURPLE = glm::vec3(0.5f, 0.0f, 0.5f);
    const inline glm::vec3 PINK = glm::vec3(1.0f, 0.0f, 0.5f);
    const inline glm::vec3 BROWN = glm::vec3(0.5f, 0.25f, 0.0f);
    const inline glm::vec3 GRAY = glm::vec3(0.5f, 0.5f, 0.5f);
    const inline glm::vec3 DARK_GRAY = glm::vec3(0.25f, 0.25f, 0.25f);
    const inline glm::vec3 MAROON = glm::vec3(0.5f, 0.0f, 0.0f);
    const inline glm::vec3 NAVY = glm::vec3(0.0f, 0.0f, 0.5f);
    const inline glm::vec3 TEAL = glm::vec3(0.0f, 0.5f, 0.5f);
    const inline glm::vec3 FOREST = glm::vec3(0.0f, 0.5f, 0.0f);
    const inline glm::vec3 OLIVE = glm::vec3(0.5f, 0.5f, 0.0f);
    const inline glm::vec3 SKY = glm::vec3(0.0f, 0.75f, 1.0f);
    const inline glm::vec3 VIOLET = glm::vec3(0.5f, 0.0f, 1.0f);
    const inline glm::vec3 SILVER = glm::vec3(0.75f, 0.75f, 0.75f);
    const inline glm::vec3 GOLD = glm::vec3(1.0f, 0.75f, 0.0f);
    const inline glm::vec3 BEIGE = glm::vec3(0.75f, 0.5f, 0.25f);
    const inline glm::vec3 TURQUOISE = glm::vec3(0.25f, 0.75f, 0.75f);
    const inline glm::vec3 MAGENTA_PINK = glm::vec3(1.0f, 0.0f, 0.75f);
    const inline glm::vec3 SALMON = glm::vec3(1.0f, 0.5f, 0.5f);
    const inline glm::vec3 LIME = glm::vec3(0.75f, 1.0f, 0.0f);
    const inline glm::vec3 INDIGO = glm::vec3(0.25f, 0.0f, 0.75f);

    const inline glm::vec3 DARK_RED = glm::vec3(0.5f, 0.0f, 0.0f);
    const inline glm::vec3 DARK_GREEN = glm::vec3(0.0f, 0.5f, 0.0f);
    const inline glm::vec3 DARK_BLUE = glm::vec3(0.0f, 0.0f, 0.5f);
    const inline glm::vec3 DARK_YELLOW = glm::vec3(0.5f, 0.5f, 0.0f);
    const inline glm::vec3 DARK_CYAN = glm::vec3(0.0f, 0.5f, 0.5f);
    const inline glm::vec3 DARK_MAGENTA = glm::vec3(0.5f, 0.0f, 0.5f);
    const inline glm::vec3 DARK_ORANGE = glm::vec3(0.5f, 0.25f, 0.0f);
    const inline glm::vec3 DARK_PURPLE = glm::vec3(0.25f, 0.0f, 0.25f);
    const inline glm::vec3 DARK_PINK = glm::vec3(0.5f, 0.0f, 0.25f);
    const inline glm::vec3 DARK_BROWN = glm::vec3(0.25f, 0.125f, 0.0f);

    const inline glm::vec3 LIGHT_RED = glm::vec3(1.0f, 0.5f, 0.5f);
    const inline glm::vec3 LIGHT_GREEN = glm::vec3(0.5f, 1.0f, 0.5f);
    const inline glm::vec3 LIGHT_BLUE = glm::vec3(0.5f, 0.5f, 1.0f);
    const inline glm::vec3 LIGHT_YELLOW = glm::vec3(1.0f, 1.0f, 0.5f);
    const inline glm::vec3 LIGHT_CYAN = glm::vec3(0.5f, 1.0f, 1.0f);
    const inline glm::vec3 LIGHT_MAGENTA = glm::vec3(1.0f, 0.5f, 1.0f);
    const inline glm::vec3 LIGHT_ORANGE = glm::vec3(1.0f, 0.75f, 0.5f);
    const inline glm::vec3 LIGHT_PURPLE = glm::vec3(0.75f, 0.5f, 0.75f);
    const inline glm::vec3 LIGHT_PINK = glm::vec3(1.0f, 0.5f, 0.75f);
    const inline glm::vec3 LIGHT_BROWN = glm::vec3(0.75f, 0.625f, 0.5f);
    const inline glm::vec3 LIGHT_GRAY = glm::vec3(0.75f, 0.75f, 0.75f);

    const inline glm::vec3 WHITE = glm::vec3(1.0f, 1.0f, 1.0f);
    const inline glm::vec3 BLACK = glm::vec3(0.0f, 0.0f, 0.0f);
}

#endif // !CONSTANT_HPP