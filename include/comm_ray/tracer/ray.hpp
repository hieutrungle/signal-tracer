#pragma once

#ifndef RAY_HPP
#define RAY_HPP

#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include <iostream>

namespace SignalTracer {
    class Ray {
    public:
        Ray() = default;
        Ray(const glm::vec3& origin, const glm::vec3& direction)
            : m_origin{ origin }
            , m_direction{ glm::normalize(direction) }
            , m_rdirection{ 1.0f / m_direction.x, 1.0f / m_direction.y, 1.0f / m_direction.z } {}

        const glm::vec3& get_origin() const { return m_origin; }
        const glm::vec3& get_direction() const { return m_direction; }
        const glm::vec3& get_rdirection() const { return m_rdirection; }

        void set_origin(const glm::vec3& origin) { m_origin = origin; }
        void set_direction(const glm::vec3& direction) {
            m_direction = glm::normalize(direction);
            m_rdirection = glm::vec3{ 1.0f / m_direction.x, 1.0f / m_direction.y, 1.0f / m_direction.z };
        }

        // negate ray
        Ray operator-() const { return Ray(m_origin, -m_direction); }
        glm::vec3 point_at(float t) const { return m_origin + t * m_direction; }

        //cout
        friend std::ostream& operator<<(std::ostream& os, const Ray& ray) {
            os << "Ray: " << glm::to_string(ray.get_origin()) << " " << glm::to_string(ray.get_direction()) << std::endl;
            return os;
        }

    private:
        glm::vec3 m_origin{};
        glm::vec3 m_direction{};
        glm::vec3 m_rdirection{};
    };

}

#endif // !RAY_HPP