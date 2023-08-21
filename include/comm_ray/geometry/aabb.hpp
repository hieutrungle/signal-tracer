#pragma once

#ifndef AABB_HPP
#define AABB_HPP

#include "constant.hpp"
#include "interval.hpp"
#include "utils.hpp"
#include "ray.hpp"

namespace SignalTracer {

    template<typename T>
    class AABBT {
    public:

        /**
         * Constructor.
         * The default constructor creates a new bounding box which contains no
         * points.
         */
        AABBT() : m_min{ Constant::INF_POS_T<T> }, m_max{ Constant::INF_NEG_T<T> }, m_extent{ m_max - m_min } {};

        /**
         * Constructor.
         * Creates a bounding box with given boxes.
         * \param b1 box 1
         * \param b2 box 2
         */
        AABBT(const AABBT<T>& b1, const AABBT<T>& b2)
            : m_min{ glm::min(b1.m_min, b2.m_min) }
            , m_max{ glm::max(b1.m_max, b2.m_max) }
            , m_extent{ m_max - m_min } {}

        /**
         * Constructor.
         * Creates a bounding box with given boxes.
         * \param b1 box 1
         * \param b2 box 2
         * \param b3 box 3
         */
        AABBT(const AABBT<T>& b1, const AABBT<T>& b2, const AABBT<T>& b3) : AABBT(b1, b2) {
            *this = AABBT(*this, b3);
        }

        /**
         * Constructor.
         * Creates a bounding box that includes a single point.
         */
        AABBT(const glm::vec3& p)
            : m_min{ p }, m_max{ p }, m_extent{ m_max - m_min } {}

        /**
         * Constructor.
         * Creates a bounding box with given 2 points.
         * \param p1 point 1
         * \param p2 point 2
         */
        AABBT(const glm::vec3& p1, const glm::vec3& p2)
            : m_min{ glm::min(p1, p2) }, m_max{ glm::max(p1, p2) }, m_extent{ m_max - m_min } {}

        /**
         * Constructor.
         * Creates a bounding box with given 2 points.
         * \param p1 point 1
         * \param p2 point 2
         * \param p3 point 3
         */
        AABBT(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
            : m_min{ glm::min(p1, glm::min(p2, p3)) }
            , m_max{ glm::max(p1, glm::max(p2, p3)) }
            , m_extent{ m_max - m_min } {}

        /**
         * Constructor.
         * Creates a bounding box with given bounds (component wise).
         */
        AABBT(const double min_x, const double min_y, const double min_z, const double max_x, const double max_y, const double max_z)
            : m_min{ min_x, min_y, min_z }
            , m_max{ max_x, max_y, max_z }
            , m_extent{ m_max - m_min } {}

        // equality operator
        bool operator==(const AABBT<T>& box) const {
            return m_min == box.m_min && m_max == box.m_max;
        }

        // inequality operator
        bool operator!=(const AABBT<T>& box) const {
            return m_min != box.m_min || m_max != box.m_max;
        }

        // cout
        friend std::ostream& operator<<(std::ostream& os, const AABBT<T>& box) {
            os << "{" << glm::to_string(box.m_min) << ", " << glm::to_string(box.m_max) << "}";
            return os;
        }

        /**
         * Expand the bounding box to include another (union).
         * If the given bounding box is contained within *this*, nothing happens.
         * Otherwise *this* is expanded to the minimum volume that contains the
         * given input.
         * \param box the bounding box to be included
         */
        void expand(const AABBT<T>& box) {
            m_min = glm::min(m_min, box.m_min);
            m_max = glm::max(m_max, box.m_max);
            m_extent = m_max - m_min;
        }

        /**
         * Expand the bounding box to include a new point in space.
         * If the given point is already inside *this*, nothing happens.
         * Otherwise *this* is expanded to a minimum volume that contains the given
         * point.
         * \param p the point to be included
         */
        void expand(const glm::vec3& p) {
            m_min = glm::min(m_min, p);
            m_max = glm::max(m_max, p);
            m_extent = m_max - m_min;
        }

        /**
         * Get the minimum corner of the bounding box.
         * \return the minimum corner of the bounding box
         */
        const glm::vec3& get_min() const {
            return m_min;
        }

        /**
         * Get the maximum corner of the bounding box.
         * \return the maximum corner of the bounding box
         */
        const glm::vec3& get_max() const {
            return m_max;
        }

        glm::vec3 get_centroid() const {
            return (m_min + m_max) * 0.5f;
        }

        /**
         * Compute the surface area of the bounding box.
         * \return surface area of the bounding box.
         */
        T calc_surface_area() const {
            return 2.0f * (m_extent.x * m_extent.y + m_extent.x * m_extent.z + m_extent.y * m_extent.z);
        }

        /**
         * Check if bounding box is empty.
         * Bounding box that has no size is considered empty. Note that since
         * bounding box are used for objects with positive volumes, a bounding
         * box of zero size (empty, or contains a single vertex) are considered
         * empty.
         */
        bool is_empty() const {
            return m_extent.x <= 0.0f || m_extent.y <= 0.0f || m_extent.z <= 0.0f;
        }

        /**
         * Ray - bbox intersection.
         * Intersects ray with bounding box, does not store shading information.
         * \param ray the ray to intersect with
         * \param interval lower bound and upper bound of intersection
         * \return POSITIVE INFINITE if not hit, otherwise, distance to hit
         */
        T hit(const Ray& ray, IntervalT<T> interval) const {

            if (ray.get_direction() == glm::vec3(0.0f)) {
                std::cout << "Ray direction is zero vector." << std::endl;
                return Constant::INF_POS_T<T>;
            }
            const auto& rdirection = ray.get_rdirection();
            const auto& origin = ray.get_origin();

            for (std::size_t i = 0; i < 3; i++) {
                if (std::fabs(ray.get_direction()[i]) <= 1e-6) {
                    continue;
                }
                T t0 = (m_min[i] - origin[i]) * rdirection[i];
                T t1 = (m_max[i] - origin[i]) * rdirection[i];

                if (rdirection[i] < 0.0f) std::swap<T>(t0, t1);

                interval.min(std::fmax(t0, interval.min()));
                interval.max(std::fmin(t1, interval.max()));
                if (interval.max() < interval.min()) {
                    return Constant::INF_POS_T<T>;
                }
            }
            return static_cast<T>(interval.min());
        }

        /**
         * Check if a point is inside the bounding box.
         * \param p the point to check
         * \return true if the point is inside the bounding box, false otherwise
         */
        bool is_contained(const glm::vec3& p) const {
            return p.x >= m_min.x && p.x <= m_max.x &&
                p.y >= m_min.y && p.y <= m_max.y &&
                p.z >= m_min.z && p.z <= m_max.z;
        }

        AABBT<T> pad() const {
            const float delta = Constant::EPSILON;
            glm::vec minn{ m_min };
            glm::vec maxx{ m_max };
            if (m_extent.x < delta) {
                minn.x -= delta;
                maxx.x += delta;
            }
            if (m_extent.y < delta) {
                minn.y -= delta;
                maxx.y += delta;
            }
            if (m_extent.z < delta) {
                minn.z -= delta;
                maxx.z += delta;
            }
            return AABBT<T>(minn, maxx);
        }

    private:
        glm::vec3 m_min{ Constant::INF_POS_T<T> };
        glm::vec3 m_max{ Constant::INF_NEG_T<T> };
        glm::vec3 m_extent{ m_max - m_min };
    };

    // define float aabb
    using AABB = AABBT<float>;
}// namespace SignalTracer

#endif // !AABB_HPP