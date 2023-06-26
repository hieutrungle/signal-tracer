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

    const float INFINITY_POS = INFINITY_POS_T<float>;
    const float INFINITY_NEG = INFINITY_NEG_T<float>;

    const float EPSILON = 1e-6;

    const inline glm::vec3 NEG_INF_VEC = glm::vec3(INFINITY_NEG);
    const inline glm::vec3 MIN_BOUND = glm::vec3(INFINITY_POS);
    const inline glm::vec3 MAX_BOUND = glm::vec3(INFINITY_NEG);
}

#endif // !CONSTANT_HPP