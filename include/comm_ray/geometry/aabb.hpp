#pragma once

#ifndef AABB_HPP
#define AABB_HPP

#include "utils.hpp"
#include "interval.hpp"
#include "ray.hpp"

namespace SignalTracer {

    template<typename T>
    class AABBT {
    public:
        AABBT() = default;

        // Initialize an AABB with three intervals
        AABBT(const IntervalT<T>& x, const IntervalT<T>& y, const IntervalT<T>& z)
            : m_intervals{ x, y, z } {}

        // Initialize an AABB with two points
        AABBT(const glm::vec3& p1, const glm::vec3& p2)
            : m_intervals{
                IntervalT<T>(std::fmin(p1.x, p2.x), std::fmax(p1.x, p2.x)) ,
                IntervalT<T>(std::fmin(p1.y, p2.y), std::fmax(p1.y, p2.y)),
                IntervalT<T>(std::fmin(p1.z, p2.z), std::fmax(p1.z, p2.z)) } {}

        AABBT(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) : AABBT(p1, p2) {
            AABBT<T> box(p2, p3);
            *this = AABBT(*this, box);
        }

        AABBT(const AABBT<T>& box, const glm::vec3 p) : AABBT(box) {
            auto min_coord = glm::vec3(box[0].min(), box[1].min(), box[2].min());
            auto max_coord = glm::vec3(box[0].max(), box[1].max(), box[2].max());
            *this = AABBT(min_coord, max_coord, p);
        }

        // union of two AABBs
        AABBT(const AABBT<T>& box1, const AABBT<T>& box2)
            : m_intervals{
                IntervalT<T>::get_union_interval(box1[0], box2[0]),
                IntervalT<T>::get_union_interval(box1[1], box2[1]),
                IntervalT<T>::get_union_interval(box1[2], box2[2]) } {}

        AABBT(const AABBT<T>& box1, const AABBT<T>& box2, const AABBT<T>& box3)
            : AABBT(AABBT(box1, box2), box3) {}

        // copy constructor
        AABBT(const AABBT<T>& box)
            : m_intervals{ box.m_intervals } {}

        // move constructor
        AABBT(AABBT<T>&& box) noexcept
            : m_intervals{ box.m_intervals } {}

        // assignment operator
        AABBT<T>& operator=(const AABBT<T>& box) {
            m_intervals = box.m_intervals;
            return *this;
        }

        // move assignment operator
        AABBT<T>& operator=(AABBT<T>&& box) noexcept {
            m_intervals = box.m_intervals;
            return *this;
        }

        bool operator==(const AABBT<T>& box) const {
            return m_intervals[0] == box.m_intervals[0] &&
                m_intervals[1] == box.m_intervals[1] &&
                m_intervals[2] == box.m_intervals[2];
        }

        bool operator!=(const AABBT<T>& box) const {
            return m_intervals[0] != box.m_intervals[0] ||
                m_intervals[1] != box.m_intervals[1] ||
                m_intervals[2] != box.m_intervals[2];
        }

        // array subscript operator
        const IntervalT<T>& operator[](int i) const {
            return m_intervals[i];
        }

        // cout
        friend std::ostream& operator<<(std::ostream& os, const AABBT<T>& box) {
            os << "{" << box.m_intervals[0] << ", " << box.m_intervals[1] << ", " << box.m_intervals[2] << "}";
            return os;
        }

        const glm::vec3 length() const {
            return glm::vec3(m_intervals[0].length(), m_intervals[1].length(), m_intervals[2].length());
        }

        bool is_empty() const {
            return m_intervals[0].is_empty() || m_intervals[1].is_empty() || m_intervals[2].is_empty();
        }

        // point is inside the AABB (including the boundary)
        bool contains(const glm::vec3& point) const {
            return m_intervals[0].contains(point.x) &&
                m_intervals[1].contains(point.y) &&
                m_intervals[2].contains(point.z);
        }

        // point is inside the AABB (excluding the boundary)
        bool surrounds(const glm::vec3& point) const {
            return m_intervals[0].surrounds(point.x) &&
                m_intervals[1].surrounds(point.y) &&
                m_intervals[2].surrounds(point.z);
        }

        bool intersect(const Ray& ray, IntervalT<T> ray_interval) const {
            if (ray.direction() == glm::vec3(0.0f)) {
                std::cout << "Ray direction is zero vector." << std::endl;
                return contains(ray.origin());
            }
            const auto inv_direction = 1.0f / ray.direction();

            for (std::size_t i = 0; i < m_intervals.size(); i++) {
                if (std::fabs(ray.direction()[i]) <= 1e-6) {
                    continue;
                }
                T t0 = (m_intervals[i].min() - ray.origin()[i]) * inv_direction[i];
                T t1 = (m_intervals[i].max() - ray.origin()[i]) * inv_direction[i];

                if (inv_direction[i] < 0.0f) std::swap<T>(t0, t1);

                ray_interval.min(std::fmax(t0, ray_interval.min()));
                ray_interval.max(std::fmin(t1, ray_interval.max()));
                if (ray_interval.max() < ray_interval.min()) {
                    return false;
                }
            }
            return true;
        }

    private:
        std::array<IntervalT<T>, 3> m_intervals{{
                IntervalT(Constant::INFINITY_POS, Constant::INFINITY_NEG),
                    IntervalT(Constant::INFINITY_POS, Constant::INFINITY_NEG),
                    IntervalT(Constant::INFINITY_POS, Constant::INFINITY_NEG)
            }
        }; // x, y, z
    };

    // define float aabb
    using AABB = AABBT<float>;
}

#endif // !AABB_HPP