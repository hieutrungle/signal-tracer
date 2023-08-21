#pragma once

#ifndef QUAD_HPP
#define QUAD_HPP

#include "intersect_record.hpp"
#include "plane.hpp"
#include "glm/glm.hpp"
#include <math.h>
#include <memory>

namespace SignalTracer {

    /// @brief A class for representing a quadrialteral
    /// @details A class for representing a quadrialteral, which is defined by
    ///     Q, a lower left corner.
    ///     u, a vector representing the first side. Q + u gives one of the norners adjacent to Q.
    ///     v, a vector representing the second side. Q + v gives one of the norners adjacent to Q.
    class Quad : public Hittable {
    public:
        // Quad() = default;
        Quad(const glm::vec3& Q, const glm::vec3& u, const glm::vec3& v);
        void set_bounding_box();

        glm::vec3 get_normal() const { return m_normal; }
        float get_d() const { return m_d; }

        AABB bounding_box() const override;
        bool is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const override;

    private:

        bool is_interior(const glm::vec3& point) const;

        glm::vec3 m_q{}; // first point
        glm::vec3 m_u{}; // first side vector
        glm::vec3 m_v{}; // second side vector
        glm::vec3 m_normal{};
        float m_d{};
        AABB m_box{};
        glm::vec3 m_w{};
    };
}

#endif // !QUAD_HPP