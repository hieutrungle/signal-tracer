#pragma once

#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "aabb.hpp"
#include "hittable.hpp"
#include "intersect_record.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "constant.hpp"
#include "material.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include <memory>

namespace SignalTracer {
    class IntersectRecord;

    class Triangle : public Hittable {

    public:
        Triangle() = default;
        Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, std::shared_ptr<Material> p_material = std::make_shared<Concrete>())
            : m_a(a), m_b(b), m_c(c), m_material_ptr(p_material) {
            update();
        }

        // copy constructor
        Triangle(const Triangle& rhs)
            : m_a(rhs.m_a), m_b(rhs.m_b), m_c(rhs.m_c)
            , m_material_ptr(rhs.m_material_ptr) {
            update();
        }

        // move constructor
        Triangle(Triangle&& rhs) noexcept
            : m_a(std::move(rhs.m_a))
            , m_b(std::move(rhs.m_b))
            , m_c(std::move(rhs.m_c))
            , m_material_ptr(std::move(rhs.m_material_ptr)) {
            update();
        }

        // assignment operator
        Triangle& operator=(const Triangle& rhs) {
            if (this == &rhs) {
                return *this;
            }
            m_a = rhs.m_a;
            m_b = rhs.m_b;
            m_c = rhs.m_c;
            m_box = rhs.m_box;
            m_material_ptr = rhs.m_material_ptr;
            return *this;
        }

        // move assignment operator
        Triangle& operator=(Triangle&& rhs) noexcept {
            if (this == &rhs) {
                return *this;
            }
            m_a = std::move(rhs.m_a);
            m_b = std::move(rhs.m_b);
            m_c = std::move(rhs.m_c);
            m_box = std::move(rhs.m_box);
            m_material_ptr = std::move(rhs.m_material_ptr);
            return *this;
        }

        bool operator==(const Triangle& rhs) const {
            for (int i = 0; i < 3; ++i) {
                if ((*this)[i] != rhs[i % 3] && (*this)[i] != rhs[(i + 1) % 3] && (*this)[i] != rhs[(i + 2) % 3]) {
                    return false;
                }
            }
            return m_material_ptr == rhs.m_material_ptr;
        }

        bool operator!=(const Triangle& rhs) const {
            return m_a != rhs.m_a || m_b != rhs.m_b || m_c != rhs.m_c || m_material_ptr != rhs.m_material_ptr;
        }

        // array subscript operator
        glm::vec3 operator[](int i) const {
            if (i == 0) return m_a;
            if (i == 1) return m_b;
            return m_c;
        }

        // cout
        std::ostream& print(std::ostream& out) const override {
            out << "Triangle: [" << glm::to_string(m_a) << ", " << glm::to_string(m_b) << ", " << glm::to_string(m_c) << "]" << std::endl;
            out << "Material: " << *m_material_ptr << std::endl;
            return out;
        }

        const glm::vec3& a() const { return m_a; }
        const glm::vec3& b() const { return m_b; }
        const glm::vec3& c() const { return m_c; }
        AABB bounding_box() const override { return m_box; }
        const std::shared_ptr<Material> p_material() const { return m_material_ptr; }

        void a(const glm::vec3& a) { m_a = a; update(); }
        void b(const glm::vec3& b) { m_b = b; update(); }
        void c(const glm::vec3& c) { m_c = c; update(); }
        void p_material(std::shared_ptr<Material> p_material) { m_material_ptr = p_material; }


        glm::vec3 get_normal() const {
            glm::vec3 edge_ab = m_b - m_a;
            glm::vec3 edge_ac = m_c - m_a;
            glm::vec3 normal = glm::normalize(glm::cross(edge_ab, edge_ac));
            return normal;
        }

        glm::vec3 get_centroid() const override {
            return (m_a + m_b + m_c) * 0.3333f;
        }

        glm::vec3 get_mirror_point(const glm::vec3& point) const {
            glm::vec3 normal = get_normal();
            auto mirror_point = point + 2.0f * glm::dot(m_b - point, normal) * normal;
            return mirror_point;
        }

        glm::vec3 get_min() const override {
            return glm::min(m_a, glm::min(m_b, m_c));
        }

        glm::vec3 get_max() const override {
            return glm::max(m_a, glm::max(m_b, m_c));
        }

        bool is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const override {
            // Tomas Moller and Ben Trumbore Algorithm
            glm::vec3 edge_ab = m_b - m_a;
            glm::vec3 edge_ac = m_c - m_a;
            glm::vec3 pvec = glm::cross(ray.get_direction(), edge_ac);
            float det = glm::dot(edge_ab, pvec);
#if defined(CULLING)
            // if the determinant is negative, the triangle is 'back facing.'
            // if the determinant is close to 0, the ray misses the triangle
            if (det < Constant::EPSILON) {
                return false;
            }
#else
            if (std::fabs(det) < Constant::EPSILON) {
                return false;
            }
#endif
            float inv_det = 1.0f / det;

            glm::vec3 tvec = ray.get_origin() - m_a;
            float u = glm::dot(tvec, pvec) * inv_det;
            if (u < 0.0f || u > 1.0f) {
                return false;
            }

            glm::vec3 qvec = glm::cross(tvec, edge_ab);
            float v = glm::dot(ray.get_direction(), qvec) * inv_det;
            if (v < 0.0f || u + v > 1.0f) {
                return false;
            }

            float t = glm::dot(edge_ac, qvec) * inv_det;
            if (t < Constant::EPSILON) {
                return false;
            }

            if (interval.contains(t)) {
                record.t = t;
                record.point = ray.point_at(t);
                record.normal = get_normal();
                record.mat_ptr = m_material_ptr;
                return true;
            }
            return false;
        }

    private:
        glm::vec3 m_a{};
        glm::vec3 m_b{};
        glm::vec3 m_c{};
        AABB m_box{};
        std::shared_ptr<Material> m_material_ptr{};

        void update() {
            m_box = AABB{ m_a, m_b, m_c };
        }
    };
}

#endif // !TRIANGLE_HPP