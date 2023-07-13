#pragma once

#ifndef SIGNAL_TRACER_HPP
#define SIGNAL_TRACER_HPP

#include "model.hpp"
#include "bvh_map.hpp"
#include "constant.hpp"
#include "reflection_record.hpp"
#include "triangle.hpp"
#include "glm/glm.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>


namespace SignalTracer {

    /*
        Instructions:

        1 Initilization Part:
        - Initialize a SignalTracer object with a vector of Mesh objects.
        - Call the tracing() function to get all reflection records.
        - Call the init_draw() function to initialize the Line objects for drawing.

        2 Drawing Part:
        - Call the draw_radio_stations() function to draw the radio stations.
        - Call the draw() function to draw the Line objects.
    */
    class SignalTracer {
    public:
        SignalTracer() = default;
        SignalTracer(const std::vector<Model>& models, int max_reflection = 2)
            : m_max_reflection{ max_reflection }
            , m_triangles{ init_triangles(models) }
            , m_bvh{ std::make_shared<BVH>(m_triangles, 0, m_triangles.size()) } {}

        SignalTracer(const std::vector<std::shared_ptr<Model>>& model_ptrs, int max_reflection = 2)
            : m_max_reflection{ max_reflection }
            , m_triangles{ init_triangles(model_ptrs) }
            , m_bvh{ std::make_shared<BVH>(m_triangles, 0, m_triangles.size()) } {}

        const std::shared_ptr<BVH>& bvh() const { return m_bvh; }
        int max_reflection() const { return m_max_reflection; }

        virtual ~SignalTracer() = default;

        // Copy constructor
        SignalTracer(const SignalTracer& signal_tracer) {
            *this = signal_tracer;
        }

        // Move constructor
        SignalTracer(SignalTracer&& signal_tracer) noexcept {
            *this = std::move(signal_tracer);
        }

        // Assignment operator
        SignalTracer& operator=(const SignalTracer& signal_tracer) {
            m_triangles = signal_tracer.m_triangles;
            m_bvh = signal_tracer.m_bvh;
            m_max_reflection = signal_tracer.m_max_reflection;
            m_is_direct_lighting = signal_tracer.m_is_direct_lighting;
            m_station_positions = signal_tracer.m_station_positions;
            return *this;
        }

        // Move assignment operator
        SignalTracer& operator=(SignalTracer&& signal_tracer) noexcept {
            m_triangles = std::move(signal_tracer.m_triangles);
            m_bvh = std::move(signal_tracer.m_bvh);
            m_max_reflection = std::move(signal_tracer.m_max_reflection);
            m_is_direct_lighting = std::move(signal_tracer.m_is_direct_lighting);
            m_station_positions = std::move(signal_tracer.m_station_positions);
            return *this;
        }

        void reset() {
            m_is_direct_lighting = false;
        }

        void set_station_positions(const glm::vec3& tx_pos, const glm::vec3& rx_pos) {
            m_station_positions.clear();
            m_station_positions.emplace_back(tx_pos);
            m_station_positions.emplace_back(rx_pos);
        }

        std::vector<glm::vec3> get_station_positions() const {
            return m_station_positions;
        }

        void tracing(glm::vec3& tx_pos, glm::vec3& rx_pos, std::vector<ReflectionRecord>& ref_records) {
            reset();
            set_station_positions(tx_pos, rx_pos);
            std::clog << "tx position: " << glm::to_string(tx_pos) << std::endl;
            std::clog << "rx position: " << glm::to_string(rx_pos) << std::endl;

            ReflectionRecord ref_record{ 0, std::vector<glm::vec3>{tx_pos}, std::vector<std::shared_ptr<Material>>{nullptr} };
            if (m_max_reflection >= 0) {
                if (is_ray_direct(tx_pos, rx_pos, ref_record)) {
                    ref_records.emplace_back(ref_record);
                }
            }
            if (m_max_reflection >= 1) {
                if (is_ray_reflected(tx_pos, rx_pos, ref_records)) {}
            }
            if (m_max_reflection >= 2) {
                if (is_ray_double_reflect(tx_pos, rx_pos, ref_records)) {}
            }
        }


        /// @brief Check if there is a direct path from tx to rx. If so, add the point to the ref_points vector of ref_record
        /// @param tx_pos 
        /// @param rx_pos 
        /// @param ref_record 
        /// @return 
        bool is_ray_direct(const glm::vec3& tx_pos, const glm::vec3& rx_pos, ReflectionRecord& ref_record) const {
            Ray ray(tx_pos, rx_pos - tx_pos);
            Interval interval{ Constant::EPSILON, glm::length(rx_pos - tx_pos) - Constant::EPSILON };
            IntersectRecord record{};
            if (!m_bvh->is_hit(ray, interval, record)) {
                ref_record.ref_points.emplace_back(rx_pos);
                ref_record.ref_materials.emplace_back(record.get_material_ptr());
                return true;
            }
            return false;
        }

        /// @brief Add the reflection records to the ref_records vector if the ray is reflected
        /// @param tx_pos 
        /// @param rx_pos 
        /// @param ref_records 
        /// @return 
        bool is_ray_reflected(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<ReflectionRecord>& ref_records) const {
            bool is_reflect = false;
            for (const auto& triangle : m_triangles) {
                glm::vec3 mirror_point = triangle->get_mirror_point(tx_pos);
                Ray ray{ mirror_point, rx_pos - mirror_point };
                Interval interval{ Constant::EPSILON, glm::length(rx_pos - mirror_point) - Constant::EPSILON };
                IntersectRecord record{};
                if (triangle->is_hit(ray, interval, record)) {
                    glm::vec3 reflective_point = record.get_point();

                    ReflectionRecord ref_record{ 1, std::vector<glm::vec3>{tx_pos}, std::vector<std::shared_ptr<Material>>{record.get_material_ptr()} };
                    if (is_ray_direct(tx_pos, reflective_point, ref_record) && is_ray_direct(reflective_point, rx_pos, ref_record)) {
                        ref_records.emplace_back(ref_record);
                        is_reflect = true;
                    }
                }
            }
            return is_reflect;
        }

        bool is_ray_double_reflect(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<ReflectionRecord>& ref_records) const {
            bool is_reflect = false;
            // Setup mirror points of tx and rx
            std::vector<glm::vec3> tx_mirror_points{};
            std::vector<glm::vec3> rx_mirror_points{};
            for (const auto& triangle : m_triangles) {
                tx_mirror_points.emplace_back(triangle->get_mirror_point(tx_pos));
                rx_mirror_points.emplace_back(triangle->get_mirror_point(rx_pos));
            }

            // Find the first reflection point
            for (std::size_t i = 0; i < m_triangles.size(); ++i) {
                glm::vec3 tx_mirror_point = tx_mirror_points[i];
                std::shared_ptr<Triangle> tx_triangle{ m_triangles[i] };

                for (std::size_t j = 0; j < m_triangles.size(); ++j) {
                    if (i == j) continue;
                    glm::vec3 rx_mirror_point = rx_mirror_points[j];
                    std::shared_ptr<Triangle> rx_triangle{ m_triangles[j] };

                    Ray ray{ tx_mirror_point, rx_mirror_point - tx_mirror_point };
                    Interval interval{ 0.0f, glm::length(rx_mirror_point - tx_mirror_point) };
                    IntersectRecord mirror_record{};
                    if (tx_triangle->is_hit(ray, interval, mirror_record)) {
                        glm::vec3 tx_reflective_point = mirror_record.get_point();

                        ray = Ray{ rx_mirror_point, tx_reflective_point - rx_mirror_point };
                        interval = Interval{ 0.0f, glm::length(tx_reflective_point - rx_mirror_point) };
                        mirror_record.clear();
                        if (rx_triangle->is_hit(ray, interval, mirror_record)) {
                            glm::vec3 rx_reflective_point = mirror_record.get_point();

                            ReflectionRecord ref_record{ 2, std::vector<glm::vec3>{tx_pos}, std::vector<std::shared_ptr<Material>>{nullptr} };
                            if (is_ray_direct(tx_pos, tx_reflective_point, ref_record) && is_ray_direct(tx_reflective_point, rx_reflective_point, ref_record) && is_ray_direct(rx_reflective_point, rx_pos, ref_record)) {
                                ref_records.emplace_back(ref_record);
                                is_reflect = true;
                            }
                        }
                    }
                }
            }
            return is_reflect;
        }

        /// @brief Calculate the free space loss in dB using the Friss formula
        /// @param distance Meters
        /// @param frequency Frequency in Hz
        /// @return Free space loss in dB
        float calc_free_space_loss(float distance, float frequency) const {
            // constant 10*log((4*pi/c)^2) = -147.55221677811664
            return 20.0f * std::log10(distance) + 20.0f * std::log10(frequency) - 147.55221677811664f;
        }

        /// @brief Calculate the received power in dBm using the Friss formula
        /// @param tx_power Power of transmitter in dBm
        /// @param tx_gain Gain of transmitter in dB
        /// @param rx_gain Gain of receiver in dB
        /// @param distance Distance between transmitter and receiver in meters
        /// @param frequency Frequency in Hz
        /// @return Received power in dBm
        float calc_received_power_freespace(float tx_power, float tx_gain, float rx_gain, float distance, float frequency) const {
            float free_space_loss = calc_free_space_loss(distance, frequency);
            return tx_power + tx_gain + rx_gain - free_space_loss;
        }

        bool is_direct_lighting() const { return m_is_direct_lighting; }

        virtual std::ostream& print(std::ostream& out) const {
            out << "Signal Tracing: \n ";
            return out;
        }
        friend std::ostream& operator<<(std::ostream& out, const SignalTracer& signal_tracer) {
            return signal_tracer.print(out);
        }

    private:

        std::vector<std::shared_ptr<Triangle>> init_triangles(const std::vector<Model>& models) {
            std::vector<std::shared_ptr<Triangle>> triangles{};
            std::vector<Vertex> vertex_buffer;
            vertex_buffer.reserve(3);
            int triangle_count{ 0 };
            int vertex_count{ 0 };
            for (const auto& model : models) {
                for (const auto& mesh : model.get_meshes()) {
                    for (const auto& idx : mesh.get_indices()) {
                        vertex_count++;
                        vertex_buffer.push_back(mesh.get_vertices()[idx]);
                        triangle_count++;
                        if (triangle_count == 3) {
                            triangles.emplace_back(std::make_shared<Triangle>(
                                vertex_buffer[0].position,
                                vertex_buffer[1].position,
                                vertex_buffer[2].position)
                            );
                            vertex_buffer.clear();
                            triangle_count = 0;
                        }
                    }
                }
            }
            std::cout << "Triangle count: " << triangles.size() << std::endl;
            std::cout << "Vertex count: " << vertex_count << std::endl;
            return triangles;
        }

        std::vector<std::shared_ptr<Triangle>> init_triangles(const std::vector<std::shared_ptr<Model>>& model_ptrs) {
            std::vector<std::shared_ptr<Triangle>> triangles{};
            std::vector<Vertex> vertex_buffer;
            vertex_buffer.reserve(3);
            int triangle_count{ 0 };
            int vertex_count{ 0 };
            for (const auto& model_ptr : model_ptrs) {
                for (const auto& mesh : model_ptr->get_meshes()) {
                    for (const auto& idx : mesh.get_indices()) {
                        vertex_count++;
                        vertex_buffer.push_back(mesh.get_vertices()[idx]);
                        triangle_count++;
                        if (triangle_count == 3) {
                            triangles.emplace_back(std::make_shared<Triangle>(
                                vertex_buffer[0].position,
                                vertex_buffer[1].position,
                                vertex_buffer[2].position)
                            );
                            vertex_buffer.clear();
                            triangle_count = 0;
                        }
                    }
                }
            }
            std::cout << "Triangle count: " << triangles.size() << std::endl;
            std::cout << "Vertex count: " << vertex_count << std::endl;
            return triangles;
        }

    private:
        int m_max_reflection{ 2 };
        std::vector<std::shared_ptr<Triangle>> m_triangles{};
        std::shared_ptr<BVH> m_bvh{};
        bool m_is_direct_lighting{ false };
        std::vector<glm::vec3> m_station_positions{};

    };
}

#endif // !SIGNAL_TRACER_HPP