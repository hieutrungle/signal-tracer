#pragma once

#ifndef COVERAGE_TRACER_HPP
#define COVERAGE_TRACER_HPP


#include "base_tracer.hpp"
#include "bvh_map.hpp"
#include "constant.hpp"
#include "coverage_map.hpp"
#include "intersect_record.hpp"
#include "path_record.hpp"
#include "triangle.hpp"
#include "quad.hpp"
#include "utils.hpp"
#include "material.hpp"
#include "model.hpp"
#include "transmitter.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include <functional>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <execution>

namespace SignalTracer {

    class CoverageTracer : public BaseTracer {
    public:
        CoverageTracer() = default;

        // CoverageTracer(const std::vector<Model>& models, int max_reflection = 2)
        //     : BaseTracer{ models } {}

        CoverageTracer(std::vector<std::reference_wrapper<Model>>& models, int max_reflection = 2, int num_rays = int(6e6))
            : BaseTracer{ models }
            , m_max_reflection{ max_reflection }
            , m_num_rays{ num_rays } {}

        ~CoverageTracer() override = default;

        // copy constructor
        CoverageTracer(const CoverageTracer& other)
            : BaseTracer{ other }
            , m_max_reflection{ other.m_max_reflection }
            , m_num_rays{ other.m_num_rays } {}

        // copy assignment
        CoverageTracer& operator=(const CoverageTracer& other) {
            BaseTracer::operator=(other);
            m_max_reflection = other.m_max_reflection;
            m_num_rays = other.m_num_rays;
            return *this;
        }

        // move constructor
        CoverageTracer(CoverageTracer&& other)
            : BaseTracer{ std::move(other) }
            , m_max_reflection{ other.m_max_reflection }
            , m_num_rays{ other.m_num_rays } {}

        // move assignment
        CoverageTracer& operator=(CoverageTracer&& other) noexcept {
            BaseTracer::operator=(std::move(other));
            m_max_reflection = other.m_max_reflection;
            m_num_rays = other.m_num_rays;
            return *this;
        }


        /// @brief Genrate the coverage map for the given model
        /// This function use a for loop for tracing rays instead of 
        /// a recursive ray tracing
        /// 0. initialize the coverage map, which is a rectangle plane
        /// 1. partition the screen into small cells
        /// 2. generate rays from the transmitters to the screen
        /// 3. trace the rays and record the reflection points, record the energy of the signal at the reflection points, 
        /// if the ray hits the coverage map plane, record the hit point, record the energy of signal at the intersection point
        /// 3.5. change the color of the cell if the cell is covered by the ray (for testing in intial phase)
        /// 4. change the ray origin to the reflection point; 
        /// change the direction to the reflection direction
        /// and repeat step 3
        /// - The final coverage map is a 2D array of cells, each cell contains a list of ray-quad intersections and the energy of signal at the intersections
        CoverageMap generate(const std::vector<Transmitter>& transmitters, float cell_size, std::vector<SignalTracer::PathRecord>* path_recs = nullptr) {
            // for (std::size_t i = 0; i < transmitters.size(); i++) {
            // }

            // testing for only one transmitter
            auto tx{ transmitters[0] };
            glm::vec3 tx_pos{ tx.get_position() };
            std::clog << "Running in sequential testing mode" << std::endl;
            std::clog << "tx position: " << glm::to_string(tx_pos) << std::endl;

            // initialize the coverage map
            Quad cm_quad{ make_coverage_quad(m_tlas.bounding_box(), 3.0f) };
            CoverageMap cm{ cm_quad, cell_size };

            // generate rays from the transmitters to the screen
            Utils::Timer timer{};
            std::vector<Ray> rays(m_num_rays);
            {
                std::vector <glm::vec3> directions{ Utils::get_fibonacci_lattice(m_num_rays) };
                std::transform(std::execution::par_unseq, directions.begin(), directions.end(), rays.begin(), [&tx_pos](const glm::vec3& dir) {return Ray{ tx_pos, dir };});
            }
            timer.execution_time();

            std::vector<SignalTracer::PathRecord> tmp_path_recs(m_num_rays);
            // ray tracing and store signal strength into a container
            timer.reset();
            float tx_power{ tx.get_power() };
            float tx_freq{ tx.get_frequency() };
            float tx_gain{ Utils::dB_to_linear(tx.get_gain()) };
            std::string polar = "TM";
            for (int i = 0; i < m_num_rays; i++) {
                tmp_path_recs[i].add_point(tx_pos);
                tmp_path_recs[i].set_signal_strength(Utils::dB_to_linear(tx_power));
                Interval interval{ Constant::EPSILON, Constant::INF_POS };

                for (int depth = 0; depth < m_max_reflection; depth++) {
                    // if the ray hits the coverage map plane, record the hit point
                    IntersectRecord cm_isect_record{};
                    IntersectRecord scene_isect_record{};
                    bool is_quad_hit{ cm_quad.is_hit(rays[i], interval, cm_isect_record) };
                    bool is_scene_hit{ m_tlas.is_hit(rays[i], interval, scene_isect_record) };

                    glm::vec3 start_pos{ tmp_path_recs[i].get_last_point() };
                    float start_strength{ tmp_path_recs[i].get_signal_strength() };
                    float start_gain{ tx_gain };
                    float end_gain{ 1.0f };

                    if (is_quad_hit && cm_isect_record.t < scene_isect_record.t) {

                        // TODO: Now: calc signal strength using Friss -> should use EM wave propagation model
                        float added_strength{ calc_friss_strength(start_pos, cm_isect_record.point, tx_freq, start_strength, start_gain, end_gain) };
                        // float added_strength{ tmp_path_recs[i].get_signal_strength() };
                        cm.add_strength(cm_isect_record.point, added_strength);
                    }

                    // if the ray hits the scene, record the hit point
                    if (is_scene_hit) {
                        tmp_path_recs[i].add_record(scene_isect_record.point, scene_isect_record.mat_ptr, scene_isect_record.tri_ptr);

                        Ray scattered_ray{};
                        glm::vec3 attenuation{};
                        if (scene_isect_record.mat_ptr->is_scattering(rays[i], scene_isect_record, attenuation, scattered_ray)) {
                            // TODO: Now: calc signal strength using Friss -> should use EM wave propagation model
                            glm::vec3 normal{ scene_isect_record.normal };
                            glm::vec3 incident_dir{ glm::normalize(rays[i].get_direction()) };
                            glm::vec3 reflected_dir{ glm::normalize(glm::reflect(incident_dir, normal)) };

                            float cos_2theta1 = glm::dot(incident_dir, reflected_dir);
                            float incident_angle = std::acos(cos_2theta1) / 2;

                            float ref_coef{ calc_reflection_coefficient(incident_angle, 1.0f, scene_isect_record.mat_ptr->calc_real_relative_permittivity(tx_freq), polar) };


                            float added_strength{ calc_friss_strength(start_pos, scene_isect_record.point, tx_freq, start_strength, start_gain, end_gain, ref_coef) };

                            tmp_path_recs[i].set_signal_strength(added_strength);
                            rays[i] = std::move(scattered_ray);
                            continue;
                        }
                        else {
                            break;
                        }
                    }
                    else {
                        if (path_recs != nullptr) {
                            if (depth != 0) {
                                auto reflected_point = rays[i].point_at(1000.0f);
                                tmp_path_recs[i].add_record(reflected_point);
                            }
                        }
                        else {
                            tmp_path_recs[i].clear();
                        }
                        break;
                    }
                }
            }
            timer.execution_time();

            if (path_recs != nullptr) {
                for (int i = 0; i < m_num_rays; i++) {
                    if (!tmp_path_recs[i].is_empty()) {
                        (*path_recs).emplace_back(tmp_path_recs[i]);
                    }
                }
            }
            cm.convert_to_dB();

            std::clog << "Coverage map generation finished" << std::endl;
            return cm;
        }

        float calc_friss_strength(const glm::vec3& start_pos, const glm::vec3& end_pos, float freq, float tx_power, float tx_gain, float rx_gain, float ref_coef = 1.0f) {
            float lambda = Constant::LIGHT_SPEED / freq;
            float dist = glm::distance(start_pos, end_pos);
            float path_loss_inv{};
            if (dist <= 1.0f) { path_loss_inv = 1.0f; }
            else { path_loss_inv = std::pow(lambda / (4 * Constant::PI * dist), 2); }
            // float strength = tx_power * tx_gain * rx_gain * std::pow(lambda / (4 * Constant::PI * dist), 2) ;
            float strength = tx_power * tx_gain * rx_gain * path_loss_inv;
            return strength * std::pow(ref_coef, 2);
        }

        Quad make_coverage_quad(const AABB& scene, float map_height = 1.5f) {
            glm::vec3 box_min{ scene.get_min() };
            glm::vec3 box_max{ scene.get_max() };

            glm::vec3 quad_q{ box_min.x, map_height, box_min.z };
            glm::vec3 quad_u{ 0.0f, 0.0f, box_max.z - box_min.z };
            glm::vec3 quad_v{ box_max.x - box_min.x, 0.0f, 0.0f };
            return Quad{ quad_q, quad_u, quad_v };
        }

        void reset() override {};
        void trace_rays(const glm::vec3& UTILS_UNUSED_PARAM(tx_pos), const glm::vec3& UTILS_UNUSED_PARAM(rx_pos), std::vector<PathRecord>& UTILS_UNUSED_PARAM(ref_records)) override {};


        static float calc_reflection_coefficient(const float& incident_angle, const float& eta1, const float& eta2, const std::string& polar = "TM") {
            float coefficient{};
            if (std::sqrt(eta1 / eta2) * std::sin(incident_angle) >= 1.0f) {
                std::cout << "Total internal reflection" << std::endl;
                coefficient = 1.0f;
            }
            else {
                float sqrt_eta1 = std::sqrt(eta1);
                float sqrt_eta2 = std::sqrt(eta2);
                float cos_theta1 = std::cos(incident_angle);
                float cos_theta2 = std::sqrt(1 - (eta1 / eta2) * std::pow(std::sin(incident_angle), 2));
                if (polar == "TM") {
                    coefficient = (sqrt_eta2 * cos_theta1 - sqrt_eta1 * cos_theta2) / (sqrt_eta2 * cos_theta1 + sqrt_eta1 * cos_theta2);
                }
                else if (polar == "TE") {
                    coefficient = (sqrt_eta1 * cos_theta1 - sqrt_eta2 * cos_theta2) / (sqrt_eta1 * cos_theta1 + sqrt_eta2 * cos_theta2);
                }
                else {
                    std::cerr << "Invalid polarization" << std::endl;
                    coefficient = 0.0f;
                }

            }
            return std::fabs(coefficient);
        }

    private:

        void trace_ray(const Ray& ray, int depth, PathRecord& path_rec) const {
            Ray cur_ray{ ray };
            IntersectRecord isect_rec{};
            Interval interval{ Constant::EPSILON, Constant::INF_POS };

            for (int i = 0; i < depth; i++) {
                if (m_tlas.is_hit(cur_ray, interval, isect_rec)) {
                    path_rec.add_record(isect_rec.point, isect_rec.mat_ptr, isect_rec.tri_ptr);
                    Ray scattered_ray{};
                    glm::vec3 attenuation{};
                    if (isect_rec.mat_ptr->is_scattering(cur_ray, isect_rec, attenuation, scattered_ray)) {
                        cur_ray = scattered_ray;
                        continue;
                    }
                    else {
                        return;
                    }
                }
                else {
                    return;
                }
            }

        }

        int m_max_reflection{ 2 };
        int m_num_rays{ static_cast<int>(6e6) };
    };

}


#endif // !COVERAGE_TRACER_HPP