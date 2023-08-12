#pragma once

#ifndef PATH_RECORD_HPP
#define PATH_RECORD_HPP

#include "triangle.hpp"
#include "material.hpp"
#include <glm/gtx/string_cast.hpp>
#include "glm/glm.hpp"
#include <iostream>
#include <memory>

namespace SignalTracer {

    class PathRecord {
    public:

        PathRecord() = default;
        PathRecord(const int& reflection_count, const std::vector<glm::vec3>& ref_points, const std::vector<std::shared_ptr<Material>>& ref_material_ptrs)
            : m_ref_count{ reflection_count }
            , m_points{ ref_points }
            , m_mat_ptrs{ ref_material_ptrs } {}

        friend std::ostream& operator<<(std::ostream& os, const PathRecord& record) {
            os << "ReflectionRecord: " << std::endl;
            os << "Reflection count: " << record.m_ref_count << std::endl;
            os << "Trace path: " << std::endl;
            for (const auto& point : record.m_points) {
                os << "\t" << glm::to_string(point) << std::endl;
            }
            os << "Trace materials: " << std::endl;
            for (const auto& material_ptr : record.m_mat_ptrs) {
                if (material_ptr == nullptr) {
                    os << "\t" << "nullptr" << std::endl;
                }
                else {
                    os << "\t" << *material_ptr;
                }
            }
            os << "Trace Triangles: " << std::endl;
            for (const auto& triangle_ptr : record.m_tri_ptrs) {
                if (triangle_ptr == nullptr) {
                    os << "\t" << "nullptr" << std::endl;
                }
                else {
                    os << "\t" << *triangle_ptr;
                }
            }
            os << "Signal loss: " << "\t" << record.m_loss << " dB" << std::endl;
            os << "Signal strength: " << "\t" << record.m_strength << " dBm" << std::endl;
            os << "Transmitting distance: " << "\t" << record.m_distance << " m" << std::endl;
            os << "Signal delay: " << "\t" << record.m_delay << " s" << std::endl;
            os << "Signal phase: " << "\t" << record.m_phase << std::endl;
            return os;
        }

        void clear() {
            m_ref_count = 0;
            m_points.clear();
            m_mat_ptrs.clear();
            m_tri_ptrs.clear();
            m_loss = 0.0f;
            m_strength = 0.0f;
            m_delay = 0.0f;
            m_distance = 0.0f;
            m_phase = 0.0f;
        }

        void set_signal_loss(const float& signal_loss) {
            m_loss = signal_loss;
        }
        void set_signal_strength(const float& signal_strength) {
            m_strength = signal_strength;
        }
        void set_signal_delay(const float& signal_delay) {
            m_delay = signal_delay;
        }
        void set_distance(const float& transmitting_distance) {
            m_distance = transmitting_distance;
        }
        void set_signal_phase(const float& signal_phase) { m_phase = signal_phase; }

        int get_reflection_count() const { return m_ref_count; }
        std::vector<glm::vec3> get_points() const { return m_points; }
        std::vector<std::shared_ptr<Material>> get_mat_ptrs() const { return m_mat_ptrs; }
        float get_signal_loss() const { return m_loss; }
        float get_signal_strength() const { return m_strength; }
        float get_signal_delay() const { return m_delay; }
        float get_distance() const { return m_distance; }
        float get_signal_phase() const { return m_phase; }

        void add_reflection_count() { m_ref_count += 1; }

        void add_point(const glm::vec3& point) {
            m_points.emplace_back(point);
        }

        void add_material_ptr(const std::shared_ptr<Material>& material_ptr) {
            m_mat_ptrs.emplace_back(material_ptr);
        }

        void add_triangle_ptr(const std::shared_ptr<Triangle>& triangle_ptr) {
            m_tri_ptrs.emplace_back(triangle_ptr);
        }

        // Add starting point or ending point
        void add_record(const glm::vec3& point) {
            add_point(point);
        }

        // Add intermediate point
        void add_record(const glm::vec3& point, const std::shared_ptr<Material>& material_ptr) {
            add_reflection_count();
            add_point(point);
            add_material_ptr(material_ptr);
        }

        void add_record(const glm::vec3& point, const std::shared_ptr<Material>& material_ptr, const std::shared_ptr<Triangle>& triangle_ptr) {
            add_reflection_count();
            add_point(point);
            add_material_ptr(material_ptr);
            add_triangle_ptr(triangle_ptr);
        }

        bool is_empty() const {
            return m_points.empty();
        }

    private:
        int m_ref_count{ 0 };
        std::vector<glm::vec3> m_points{};
        std::vector<std::shared_ptr<Material>> m_mat_ptrs{};
        std::vector<std::shared_ptr<Triangle>> m_tri_ptrs;
        float m_loss{};
        float m_strength{};
        float m_delay{};
        float m_distance{};
        float m_phase{};
        std::vector<glm::vec3> m_doas{}; // Direction of arrival
        std::vector<glm::vec3> m_dods{}; // Direction of departure
        glm::mat2 m_transition_mat{}; // Transition matrix
    };
}

#endif // !PATH_RECORD_HPP