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
        Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, std::shared_ptr<Material> p_material = std::make_shared<Concrete>());

        // copy constructor
        Triangle(const Triangle& rhs);

        // move constructor
        Triangle(Triangle&& rhs) noexcept;

        // assignment operator
        Triangle& operator=(const Triangle& rhs);

        // move assignment operator
        Triangle& operator=(Triangle&& rhs) noexcept;

        bool operator==(const Triangle& rhs) const;
        bool operator!=(const Triangle& rhs) const;

        // array subscript operator
        glm::vec3 operator[](int i) const;

        // cout
        std::ostream& print(std::ostream& out) const override;

        // Get edges
        const glm::vec3& a() const;
        const glm::vec3& b() const;
        const glm::vec3& c() const;
        AABB bounding_box() const override;
        const std::shared_ptr<Material> p_material() const;

        // Set edges
        void a(const glm::vec3& a);
        void b(const glm::vec3& b);
        void c(const glm::vec3& c);
        void p_material(std::shared_ptr<Material> p_material);


        glm::vec3 get_normal() const;
        glm::vec3 get_centroid() const override;
        glm::vec3 get_mirror_point(const glm::vec3& point) const;

        /// @brief Get the minimum point of the bounding box of the triangle.
        /// @return The minimum point of the bounding box of the triangle.
        glm::vec3 get_min() const override;

        /// @brief Get the maximum point of the bounding box of the triangle.
        /// @return The maximum point of the bounding box of the triangle.
        glm::vec3 get_max() const override;

        bool is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const override;

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