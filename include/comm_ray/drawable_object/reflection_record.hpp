#pragma once

#ifndef REFLECTION_RECORD_HPP
#define REFLECTION_RECORD_HPP

#include "material.hpp"
#include <glm/gtx/string_cast.hpp>
#include "glm/glm.hpp"
#include <iostream>
#include <memory>

namespace SignalTracer {

    class ReflectionRecord {
    public:

        ReflectionRecord() = default;
        ReflectionRecord(const int& reflection_count, const std::vector<glm::vec3>& ref_points, const std::vector<std::shared_ptr<Material>>& ref_material_ptrs, const float& signal_loss = 0.0f, const float& signal_strength = 0.0f, const float& signal_delay = 0.0f, const float& transmitting_distance = 0.0f, const float& signal_phase = 0.0f)
            : m_reflection_count{ reflection_count }
            , m_ref_points{ ref_points }
            , m_ref_material_ptrs{ ref_material_ptrs }
            , m_signal_loss{ signal_loss }
            , m_signal_strength{ signal_strength }
            , m_signal_delay{ signal_delay }
            , m_transmitting_distance{ transmitting_distance }
            , m_signal_phase{ signal_phase } {}

        friend std::ostream& operator<<(std::ostream& os, const ReflectionRecord& record) {
            os << "ReflectionRecord: " << std::endl;
            os << "Reflection count: " << record.m_reflection_count << std::endl;
            os << "Trace path: " << std::endl;
            for (const auto& point : record.m_ref_points) {
                os << "\t" << glm::to_string(point) << std::endl;
            }
            os << "Trace materials: " << std::endl;
            for (const auto& material_ptr : record.m_ref_material_ptrs) {
                if (material_ptr == nullptr) {
                    os << "\t" << "nullptr" << std::endl;
                }
                else {
                    os << "\t" << *material_ptr;
                }
            }
            os << "Signal loss: " << "\t" << record.m_signal_loss << " dB" << std::endl;
            os << "Signal strength: " << "\t" << record.m_signal_strength << " dBm" << std::endl;
            os << "Transmitting distance: " << "\t" << record.m_transmitting_distance << " m" << std::endl;
            os << "Signal delay: " << "\t" << record.m_signal_delay << " s" << std::endl;
            os << "Signal phase: " << "\t" << record.m_signal_phase << std::endl;
            return os;
        }

        void clear() {
            m_reflection_count = 0;
            m_ref_points.clear();
            m_ref_material_ptrs.clear();
            m_signal_loss = 0.0f;
            m_signal_strength = 0.0f;
            m_signal_delay = 0.0f;
            m_transmitting_distance = 0.0f;
            m_signal_phase = 0.0f;
        }

        void set_reflection_count(const float& reflection_count) {
            m_reflection_count = reflection_count;
        }
        void set_ref_points(const std::vector<glm::vec3>& ref_points) {
            m_ref_points = ref_points;
        }
        void set_ref_material_ptrs(const std::vector<std::shared_ptr<Material>>& ref_material_ptrs) {
            m_ref_material_ptrs = ref_material_ptrs;
        }
        void set_signal_loss(const float& signal_loss) {
            m_signal_loss = signal_loss;
        }
        void set_signal_strength(const float& signal_strength) {
            m_signal_strength = signal_strength;
        }
        void set_signal_delay(const float& signal_delay) {
            m_signal_delay = signal_delay;
        }
        void set_transmitting_distance(const float& transmitting_distance) {
            m_transmitting_distance = transmitting_distance;
        }
        void set_signal_phase(const float& signal_phase) {
            m_signal_phase = signal_phase;
        }

        int get_reflection_count() const {
            return m_reflection_count;
        }
        std::vector<glm::vec3> get_ref_points() const {
            return m_ref_points;
        }
        std::vector<std::shared_ptr<Material>> get_ref_material_ptrs() const {
            return m_ref_material_ptrs;
        }
        float get_signal_loss() const {
            return m_signal_loss;
        }
        float get_signal_strength() const {
            return m_signal_strength;
        }
        float get_signal_delay() const {
            return m_signal_delay;
        }
        float get_transmitting_distance() const {
            return m_transmitting_distance;
        }
        float get_signal_phase() const {
            return m_signal_phase;
        }

        void add_reflection_count() {
            m_reflection_count += 1;
        }

        void add_point(const glm::vec3& point) {
            m_ref_points.emplace_back(point);
        }
        void add_material_ptr(const std::shared_ptr<Material>& material_ptr) {
            m_ref_material_ptrs.emplace_back(material_ptr);
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

        bool is_empty() const {
            return m_ref_points.empty();
        }

    private:
        int m_reflection_count{ 0 };
        std::vector<glm::vec3> m_ref_points{};
        std::vector<std::shared_ptr<Material>> m_ref_material_ptrs{};
        float m_signal_loss{};
        float m_signal_strength{};
        float m_signal_delay{};
        float m_transmitting_distance{};
        float m_signal_phase{};
    };
}

#endif // !REFLECTION_RECORD_HPP