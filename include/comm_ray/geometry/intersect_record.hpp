#pragma once

#ifndef INTERSECT_RECORD_HPP
#define INTERSECT_RECORD_HPP

#include "ray.hpp"
#include "constant.hpp"
#include "glm/glm.hpp"
#include <iostream>
#include <memory>

namespace SignalTracer {
    class Material;

    /*
        ----------------------------------------
        IntersectRecord
        A container to store the information of the intersection
        ----------------------------------------
    */
    class IntersectRecord {
    public:
        IntersectRecord() : m_point{ Constant::INFINITY_NEG }, m_normal{}, m_material_ptr{ nullptr }, m_t{ Constant::INFINITY_POS }, m_front_face{ false } {};
        IntersectRecord(glm::vec3 point, glm::vec3 normal, std::shared_ptr<Material> material_ptr, float t, bool front_face)
            : m_point(point)
            , m_normal(normal)
            , m_material_ptr(material_ptr)
            , m_t(t)
            , m_front_face(front_face) {};

        // copy constructor
        IntersectRecord(const IntersectRecord& record) {
            m_point = record.get_point();
            m_normal = record.get_normal();
            m_material_ptr = record.get_material_ptr();
            m_t = record.get_t();
            m_front_face = record.get_front_face();
        }

        // copy assignment
        IntersectRecord& operator=(const IntersectRecord& record) {
            m_point = record.get_point();
            m_normal = record.get_normal();
            m_material_ptr = record.get_material_ptr();
            m_t = record.get_t();
            m_front_face = record.get_front_face();
            return *this;
        }

        // move constructor
        IntersectRecord(IntersectRecord&& record) noexcept {
            m_point = std::move(record.get_point());
            m_normal = std::move(record.get_normal());
            m_material_ptr = std::move(record.get_material_ptr());
            m_t = std::move(record.get_t());
            m_front_face = std::move(record.get_front_face());
        }

        // move assignment
        IntersectRecord& operator=(IntersectRecord&& record) noexcept {
            m_point = std::move(record.get_point());
            m_normal = std::move(record.get_normal());
            m_material_ptr = std::move(record.get_material_ptr());
            m_t = std::move(record.get_t());
            m_front_face = std::move(record.get_front_face());
            return *this;
        }

        // equality
        bool operator==(const IntersectRecord& record) const {
            return m_point == record.get_point() &&
                m_normal == record.get_normal() &&
                m_material_ptr == record.get_material_ptr() &&
                m_t == record.get_t() &&
                m_front_face == record.get_front_face();
        }

        // inequality
        bool operator!=(const IntersectRecord& record) const {
            return !(*this == record);
        }

        const glm::vec3 get_point() const { return m_point; };
        const glm::vec3 get_normal() const { return m_normal; };
        const std::shared_ptr<Material> get_material_ptr() const { return m_material_ptr; };
        float get_t() const { return m_t; };
        bool get_front_face() const { return m_front_face; };

        void set_point(glm::vec3 point) { m_point = point; };
        void set_normal(glm::vec3 normal) { m_normal = glm::normalize(normal); };
        void set_material_ptr(std::shared_ptr<Material> material_ptr) { m_material_ptr = material_ptr; };
        void set_t(float t) { m_t = t; };
        void set_front_face(bool front_face) { m_front_face = front_face; };

        // r: incomming Ray
        // outward_normal: normal vector of the surface
        inline void set_face_normal(const Ray& ray, const glm::vec3& outward_normal) {
            m_front_face = glm::dot(ray.get_direction(), outward_normal) < 0;
            m_normal = m_front_face ? outward_normal : -outward_normal;
        }

        // cout
        friend std::ostream& operator<<(std::ostream& os, const IntersectRecord& record) {
            os << "IntersectRecord: " << glm::to_string(record.get_point()) << std::endl;

            // " " << glm::to_string(record.get_normal()) << " " << record.get_t() << std::endl;
            return os;
        }

        // clear the record to default
        void clear() {
            *this = {};
        }

        bool is_empty() {
            return *this == IntersectRecord{};
        }

    private:
        glm::vec3 m_point{ Constant::INFINITY_NEG };
        glm::vec3 m_normal{};
        std::shared_ptr<Material> m_material_ptr{ nullptr };
        float m_t{ Constant::INFINITY_POS };
        bool m_front_face{};
    };
}

#endif // !INTERSECT_RECORD_HPP