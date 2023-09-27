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

        //     /**
        //      * Constructor.
        //      * The default constructor creates a new bounding box which contains no
        //      * points.
        //      */
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
         */
        bool is_hit(const Ray& ray, IntervalT<T> ray_interval) const {
            if (ray.get_direction() == glm::vec3(0.0f)) {
                std::cout << "Ray direction is zero vector." << std::endl;
                return false;
            }
            const auto inv_direction = 1.0f / ray.get_direction();

            for (std::size_t i = 0; i < 3; i++) {
                if (std::fabs(ray.get_direction()[i]) <= 1e-6) {
                    continue;
                }
                T t0 = (m_min[i] - ray.get_origin()[i]) * inv_direction[i];
                T t1 = (m_max[i] - ray.get_origin()[i]) * inv_direction[i];

                if (inv_direction[i] < 0.0f) std::swap<T>(t0, t1);

                ray_interval.min(std::fmax(t0, ray_interval.min()));
                ray_interval.max(std::fmin(t1, ray_interval.max()));
                if (ray_interval.max() < ray_interval.min()) {
                    return false;
                }
            }
            return true;
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

            T t0 = (m_min.x - origin.x) * rdirection.x;
            T t1 = (m_max.x - origin.x) * rdirection.x;
            T t2 = (m_min.y - origin.y) * rdirection.y;
            T t3 = (m_max.y - origin.y) * rdirection.y;
            T t4 = (m_min.z - origin.z) * rdirection.z;
            T t5 = (m_max.z - origin.z) * rdirection.z;

            T t_min = std::fmax(std::fmax(std::fmin(t0, t1), std::fmin(t2, t3)), std::fmin(t4, t5));
            T t_max = std::fmin(std::fmin(std::fmax(t0, t1), std::fmax(t2, t3)), std::fmax(t4, t5));
            t_min = std::fmax(t_min, interval.min());
            t_max = std::fmin(t_max, interval.max());
            if (t_max < t_min || t_max < 0.0f) {
                return Constant::INF_POS_T<T>;
            }
            return t_min;
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
        // Total 36 bytes
        glm::vec3 m_min{ Constant::INF_POS_T<T> };
        glm::vec3 m_max{ Constant::INF_NEG_T<T> };
        glm::vec3 m_extent{ m_max - m_min };
    };

    // define float aabb
    using AABB = AABBT<float>;


    // typedef struct AABB {
    //     glm::vec3 min;
    //     glm::vec3 max;
    //     glm::vec3 extent;
    // } AABB_t;

    // AABB_t make_aabb(glm::vec3 min, glm::vec3 max) {
    //     AABB_t aabb;
    //     aabb.min = min;
    //     aabb.max = max;
    //     aabb.extent = max - min;
    //     return aabb;
    // }

    // AABB_t make_aabb_from_points(glm::vec3* points, int num_points) {
    //     AABB_t aabb;
    //     aabb.min = glm::vec3(Constant::INF_POS);
    //     aabb.max = glm::vec3(Constant::INF_NEG);
    //     for (int i = 0; i < num_points; i++) {
    //         aabb.min.x = fmin(aabb.min.x, points[i].x);
    //         aabb.min.y = fmin(aabb.min.y, points[i].y);
    //         aabb.min.z = fmin(aabb.min.z, points[i].z);
    //         aabb.max.x = fmax(aabb.max.x, points[i].x);
    //         aabb.max.y = fmax(aabb.max.y, points[i].y);
    //         aabb.max.z = fmax(aabb.max.z, points[i].z);
    //     }
    //     aabb.extent = aabb.max - aabb.min;
    //     return aabb;
    // }

    // void aabb_expand(AABB_t* aabb, glm::vec3 point) {
    //     aabb->min.x = fmin(aabb->min.x, point.x);
    //     aabb->min.y = fmin(aabb->min.y, point.y);
    //     aabb->min.z = fmin(aabb->min.z, point.z);
    //     aabb->max.x = fmax(aabb->max.x, point.x);
    //     aabb->max.y = fmax(aabb->max.y, point.y);
    //     aabb->max.z = fmax(aabb->max.z, point.z);
    //     aabb->extent = aabb->max - aabb->min;
    // }

    // bool aabb_contains(const AABB_t* aabb, glm::vec3 point) {
    //     return aabb->min.x <= point.x && point.x <= aabb->max.x &&
    //         aabb->min.y <= point.y && point.y <= aabb->max.y &&
    //         aabb->min.z <= point.z && point.z <= aabb->max.z;
    // }

    // glm::vec3 aabb_centroid(const AABB_t* aabb) {
    //     return glm::vec3((aabb->min.x + aabb->max.x) * 0.5f,
    //         (aabb->min.y + aabb->max.y) * 0.5f,
    //         (aabb->min.z + aabb->max.z) * 0.5f);
    // }

    // float aabb_hit(const AABB_t* aabb, const Ray_t* ray, Interval_t interval) {
    //     if (ray->direction.x == 0.0f && ray->direction.y == 0.0f &&
    //         ray->direction.z == 0.0f) {
    //         return Constant::INF_POS;
    //     }

    //     float t0 = (aabb->min.x - ray->origin.x) * ray->rdirection.x;
    //     float t1 = (aabb->max.x - ray->origin.x) * ray->rdirection.x;
    //     float t2 = (aabb->min.y - ray->origin.y) * ray->rdirection.y;
    //     float t3 = (aabb->max.y - ray->origin.y) * ray->rdirection.y;
    //     float t4 = (aabb->min.z - ray->origin.z) * ray->rdirection.z;
    //     float t5 = (aabb->max.z - ray->origin.z) * ray->rdirection.z;

    //     float t_min = fmax(fmax(fmin(t0, t1), fmin(t2, t3)), fmin(t4, t5));
    //     float t_max = fmin(fmin(fmax(t0, t1), fmax(t2, t3)), fmax(t4, t5));
    //     t_min = fmax(t_min, interval.t_min);
    //     t_max = fmin(t_max, interval.t_max);
    //     if (t_max < t_min || t_max < 0.0f) {
    //         return Constant::INF_POS;
    //     }
    //     return t_min;
    // }

    // float aabb_surface_area(AABB_t* aabb) {
    //     glm::vec3 extent = aabb->extent;
    //     return 2.0f *
    //         (extent.x * extent.y + extent.y * extent.z + extent.z * extent.x);
    // }
}// namespace SignalTracer

#endif // !AABB_HPP