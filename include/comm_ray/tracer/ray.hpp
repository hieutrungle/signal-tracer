#pragma once

#ifndef RAY_HPP
#define RAY_HPP

#include <glm/glm.hpp>

namespace SignalTracer {
    class Ray {
    public:
        Ray() = default;
        Ray(const glm::vec3& origin, const glm::vec3& direction)
            : m_origin(origin), m_direction(glm::normalize(direction)) {}

        const glm::vec3& origin() const { return m_origin; }
        const glm::vec3& direction() const { return m_direction; }

        void origin(const glm::vec3& origin) { m_origin = origin; }
        void direction(const glm::vec3& direction) { m_direction = glm::normalize(direction); }

        // negate ray
        Ray operator-() const { return Ray(m_origin, -m_direction); }
        glm::vec3 point_at(float t) const { return m_origin + t * m_direction; }

    private:
        glm::vec3 m_origin{};
        glm::vec3 m_direction{};
    };

}

#endif // !RAY_HPP