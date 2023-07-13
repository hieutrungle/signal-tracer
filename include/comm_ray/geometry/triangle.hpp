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
            m_edge_ab = rhs.m_edge_ab;
            m_edge_ac = rhs.m_edge_ac;
            m_normal = rhs.m_normal;
            m_aabb = rhs.m_aabb;
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
            m_edge_ab = std::move(rhs.m_edge_ab);
            m_edge_ac = std::move(rhs.m_edge_ac);
            m_normal = std::move(rhs.m_normal);
            m_aabb = std::move(rhs.m_aabb);
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

        // array subscript operator with assignment
        // eg. triangle[0] = glm::vec3{1.0f, 1.0f, 1.0f};
        // glm::vec3& operator[](int i) {
        //     if (i == 0) { update(); return m_a; };
        //     if (i == 1) return m_b;
        //     return m_c;
        // }

        // cout
        std::ostream& print(std::ostream& out) const override {
            out << "Triangle: [" << glm::to_string(m_a) << ", " << glm::to_string(m_b) << ", " << glm::to_string(m_c) << "]" << std::endl;
            out << "Material: " << *m_material_ptr << std::endl;
            return out;
        }

        const glm::vec3& a() const { return m_a; }
        const glm::vec3& b() const { return m_b; }
        const glm::vec3& c() const { return m_c; }
        const glm::vec3& normal() const { return m_normal; }
        AABB bounding_box() const override { return m_aabb; }
        const std::shared_ptr<Material> p_material() const { return m_material_ptr; }

        void a(const glm::vec3& a) { m_a = a; update(); }
        void b(const glm::vec3& b) { m_b = b; update(); }
        void c(const glm::vec3& c) { m_c = c; update(); }
        void p_material(std::shared_ptr<Material> p_material) { m_material_ptr = p_material; }

        glm::vec3 centroid() const {
            return (m_a + m_b + m_c) / 3.0f;
        }

        glm::vec3 get_mirror_point(const glm::vec3& point) const {
            auto mirror_point = point + 2.0f * glm::dot(m_b - point, m_normal) * m_normal;
            return mirror_point;
        }

        bool is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const override {
            // Tomas Moller and Ben Trumbore Algorithm
            glm::vec3 pvec = glm::cross(ray.direction(), m_edge_ac);
            float det = glm::dot(m_edge_ab, pvec);
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

            glm::vec3 tvec = ray.origin() - m_a;
            float u = glm::dot(tvec, pvec) * inv_det;
            if (u < 0.0f || u > 1.0f) {
                return false;
            }

            glm::vec3 qvec = glm::cross(tvec, m_edge_ab);
            float v = glm::dot(ray.direction(), qvec) * inv_det;
            if (v < 0.0f || u + v > 1.0f) {
                return false;
            }

            float t = glm::dot(m_edge_ac, qvec) * inv_det;
            if (t < Constant::EPSILON) {
                return false;
            }

            if (interval.contains(t)) {
                record.set_t(t);
                record.set_point(ray.point_at(t));
                record.set_face_normal(ray, m_normal);
                record.set_material_ptr(m_material_ptr);
                return true;
            }
            return false;
        }

    private:
        glm::vec3 m_a{};
        glm::vec3 m_b{};
        glm::vec3 m_c{};
        glm::vec3 m_edge_ab{};
        glm::vec3 m_edge_ac{};
        glm::vec3 m_normal{};
        AABB m_aabb{};
        std::shared_ptr<Material> m_material_ptr{};

        void update() {
            m_edge_ab = m_b - m_a;
            m_edge_ac = m_c - m_a;
            m_normal = glm::normalize(glm::cross(m_edge_ab, m_edge_ac));
            m_aabb = AABB{ m_a, m_b, m_c };
        }
    };
}

#endif // !TRIANGLE_HPP