#pragma once

#ifndef COVERAGE_TRACER_HPP
#define COVERAGE_TRACER_HPP


#include "base_tracer.hpp"
#include "bvh_map.hpp"
#include "cl_utils.hpp"
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
#include "omp.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include <cmath>
#include <execution>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>

#include "triangle.hpp"

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
        CoverageMap generate(const std::vector<Transmitter>& transmitters, float cell_size, std::vector<SignalTracer::PathRecord>* path_recs = nullptr, const std::string& method = "friss") {
            // CoverageMap cm{ generate_seq(transmitters,cell_size, path_recs, method) };
            CoverageMap cm{ generate_par(transmitters,cell_size, path_recs, method) };
            // CoverageMap cm{ generate_opencl(transmitters,cell_size, path_recs, method) };
            return cm;

        }

        CoverageMap generate_opencl(const std::vector<Transmitter>& transmitters, float cell_size, std::vector<SignalTracer::PathRecord>* path_recs = nullptr, const std::string& method = "friss") {
            // for (std::size_t i = 0; i < transmitters.size(); i++) {
            // }

            // testing for only one transmitter
            auto tx{ transmitters[0] };
            glm::vec3 tx_pos{ tx.get_position() };
            std::clog << "Running in parallel testing mode" << std::endl;
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

            std::vector<SignalTracer::PathRecord> tmp_path_recs(m_num_rays);
            float tx_power{ tx.get_power() };
            float tx_freq{ tx.get_frequency() };
            float tx_gain{ Utils::dB_to_linear(tx.get_gain()) };
            std::string polar = "TM";


            /*
                ----------------------------------
                OpenCL
                ----------------------------------
            */
            std::filesystem::path current_path{ std::filesystem::current_path() };
            CLUtils::Kernel cl_tracer{ current_path / "cl_source", "ray_tracing.cl", "ray_trace", true };
            cl_tracer.init(true);
            // int cell_size1 = 3;
            // // std::vector<float> h_a{ 1.0f, 2.0f, 3.0f };
            // // CLUtils::Buffer d_a_buffer{ Utils::vector_sizeof(h_a), h_a.data(), CL_MEM_READ_ONLY };
            // // CLUtils::Buffer d_b_buffer{ Utils::vector_sizeof(h_b), h_b.data(), CL_MEM_READ_ONLY };
            // // CLUtils::Buffer d_c_buffer{ Utils::vector_sizeof(h_c), h_c.data(), CL_MEM_WRITE_ONLY };

            int num_triangles = 2;
            std::vector<IntersectRecord> h_isect_records(num_triangles);
            std::vector<Triangle> h_triangles(num_triangles);
            h_triangles[0] = Triangle{ glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f } };
            h_triangles[1] = Triangle{ glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f } };

            std::vector<Ray> h_rays{ Ray{ glm::vec3{ 0.0f, 0.5f, 1.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f } }, Ray{ glm::vec3{ 0.0f, 0.5f, 1.0f }, glm::vec3{ 0.0f, -1.0f, -1.0f } } };

            std::cout << "size of ray: " << sizeof(Ray) << std::endl;
            std::cout << "size of intersect record: " << sizeof(IntersectRecord) << std::endl;
            std::cout << "size of triangle: " << sizeof(Triangle) << std::endl;
            std::cout << "size of AABB: " << sizeof(AABB) << std::endl;
            std::cout << "size of Material: " << sizeof(Material) << std::endl;


            CLUtils::Buffer record_buffer{ Utils::vector_sizeof(h_isect_records), h_isect_records.data(), CL_MEM_READ_WRITE };
            CLUtils::Buffer triangle_buffer{ Utils::vector_sizeof(h_triangles), h_triangles.data(), CL_MEM_READ_ONLY };
            CLUtils::Buffer ray_buffer{ Utils::vector_sizeof(h_rays), h_rays.data(), CL_MEM_READ_WRITE };
            // CLUtils::Buffer interval_buffer{ sizeof(Interval), &ray_interval, CL_MEM_READ_ONLY };

            cl::Event event;
            // // double eslapsed = 0.0;
            // // d_a_buffer.copy_to_device();
            // // d_b_buffer.copy_to_device();
            // // cl_tracer.set_args(n, d_a_buffer, d_b_buffer, d_c_buffer);
            record_buffer.copy_to_device();
            triangle_buffer.copy_to_device();
            ray_buffer.copy_to_device();

            cl_tracer.set_args(record_buffer, triangle_buffer, ray_buffer, Constant::EPSILON, Constant::INF_POS);
            cl_tracer.run(num_triangles, 1, 1, &event);
            record_buffer.copy_from_device();
            for (int i = 0; i < num_triangles; i++) {
                std::cout << h_isect_records[i] << std::endl;
            }
            // event.wait();
            // eslapsed = event.getProfilingInfo<CL_PROFILING_COMMAND_END>() - event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
            cl::finish();

            exit(EXIT_SUCCESS);


            for (int i = 0; i < m_num_rays; i++) {
                tmp_path_recs[i].add_point(tx_pos);
                tmp_path_recs[i].set_signal_strength(Utils::dB_to_linear(tx_power));
                Interval interval{ Constant::EPSILON, Constant::INF_POS };

                for (int depth = 0; depth < m_max_reflection; depth++) {
                    // if the ray hits the coverage map plane, record the hit point
                    IntersectRecord cm_isect_record{};
                    IntersectRecord scene_isect_record{};
                    bool is_quad_hit{ cm_quad.is_hit(rays[i], interval, cm_isect_record) };

                    // TODO: do the ray tracing in opencl
                    bool is_scene_hit{ m_tlas.is_hit(rays[i], interval, scene_isect_record) };

                    glm::vec3 start_pos{ tmp_path_recs[i].get_last_point() };
                    float start_strength{ tmp_path_recs[i].get_signal_strength() };
                    float start_gain{ tx_gain };
                    float end_gain{ 1.0f };

                    if (is_quad_hit && cm_isect_record.t < scene_isect_record.t) {

                        // TODO: Now: calc signal strength using Friss -> should use EM wave propagation model
                        if (method == "friss") {
                            float added_strength{ calc_friss_strength(start_pos, cm_isect_record.point, tx_freq, start_strength, start_gain, end_gain) };
                            cm.add_strength(cm_isect_record.point, added_strength);
                        }
                    }

                    // if the ray hits the scene, record the hit point
                    if (is_scene_hit) {
                        tmp_path_recs[i].add_record(scene_isect_record.point, scene_isect_record.tri_ptr->get_mat_ptr(), scene_isect_record.tri_ptr);

                        Ray scattered_ray{};
                        glm::vec3 attenuation{};
                        if (scene_isect_record.tri_ptr->get_mat_ptr()->is_scattering(rays[i], scene_isect_record, attenuation, scattered_ray)) {

                            // TODO: Now: calc signal strength using Friss -> should use EM wave propagation model
                            if (method == "friss") {
                                glm::vec3 normal{ scene_isect_record.normal };
                                glm::vec3 incident_dir{ glm::normalize(rays[i].get_direction()) };
                                glm::vec3 reflected_dir{ glm::normalize(glm::reflect(incident_dir, normal)) };

                                float cos_2theta1 = glm::dot(incident_dir, reflected_dir);
                                float incident_angle = std::acos(cos_2theta1) / 2;
                                float ref_coef{ calc_reflection_coefficient(incident_angle, 1.0f, scene_isect_record.tri_ptr->get_mat_ptr()->calc_real_relative_permittivity(tx_freq), polar) };

                                float added_strength{ calc_friss_strength(start_pos, scene_isect_record.point, tx_freq, start_strength, start_gain, end_gain, ref_coef) };

                                tmp_path_recs[i].set_signal_strength(added_strength);
                            }

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

            std::clog << "Coverage map is generated" << std::endl;
            return cm;
        }

        CoverageMap generate_seq(const std::vector<Transmitter>& transmitters, float cell_size, std::vector<SignalTracer::PathRecord>* path_recs = nullptr, const std::string& method = "friss") {
            // for (std::size_t i = 0; i < transmitters.size(); i++) {
            // }

            // testing for only one transmitter
            auto tx{ transmitters[0] };
            glm::vec3 tx_pos{ tx.get_position() };
            std::clog << "Running in parallel testing mode" << std::endl;
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

            std::vector<SignalTracer::PathRecord> tmp_path_recs(m_num_rays);
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
                        if (method == "friss") {
                            float added_strength{ calc_friss_strength(start_pos, cm_isect_record.point, tx_freq, start_strength, start_gain, end_gain) };
                            cm.add_strength(cm_isect_record.point, added_strength);
                        }

                    }

                    // if the ray hits the scene, record the hit point
                    if (is_scene_hit) {
                        tmp_path_recs[i].add_record(scene_isect_record.point, scene_isect_record.tri_ptr->get_mat_ptr(), scene_isect_record.tri_ptr);

                        Ray scattered_ray{};
                        glm::vec3 attenuation{};
                        if (scene_isect_record.tri_ptr->get_mat_ptr()->is_scattering(rays[i], scene_isect_record, attenuation, scattered_ray)) {

                            // TODO: Now: calc signal strength using Friss -> should use EM wave propagation model
                            if (method == "friss") {
                                glm::vec3 normal{ scene_isect_record.normal };
                                glm::vec3 incident_dir{ glm::normalize(rays[i].get_direction()) };
                                glm::vec3 reflected_dir{ glm::normalize(glm::reflect(incident_dir, normal)) };

                                float cos_2theta1 = glm::dot(-incident_dir, reflected_dir);
                                float incident_angle = std::acos(cos_2theta1) / 2;
                                float ref_coef{ calc_reflection_coefficient(incident_angle, 1.0f, scene_isect_record.tri_ptr->get_mat_ptr()->calc_real_relative_permittivity(tx_freq), polar) };

                                float added_strength{ calc_friss_strength(start_pos, scene_isect_record.point, tx_freq, start_strength, start_gain, end_gain, ref_coef) };

                                tmp_path_recs[i].set_signal_strength(added_strength);
                            }

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

            std::clog << "Coverage map is generated" << std::endl;
            return cm;
        }

        CoverageMap generate_par(const std::vector<Transmitter>& transmitters, float cell_size, std::vector<SignalTracer::PathRecord>* path_recs = nullptr, const std::string& method = "friss") {
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

            std::vector<SignalTracer::PathRecord> tmp_path_recs(m_num_rays);
            float tx_power{ tx.get_power() };
            float tx_freq{ tx.get_frequency() };
            float tx_gain{ Utils::dB_to_linear(tx.get_gain()) };
            std::string polar = "TM";
#pragma omp parallel for
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
                        if (method == "friss") {
                            float added_strength{ calc_friss_strength(start_pos, cm_isect_record.point, tx_freq, start_strength, start_gain, end_gain) };
                            cm.add_strength(cm_isect_record.point, added_strength);
                        }

                    }

                    // if the ray hits the scene, record the hit point
                    if (is_scene_hit) {
                        tmp_path_recs[i].add_record(scene_isect_record.point, scene_isect_record.tri_ptr->get_mat_ptr(), scene_isect_record.tri_ptr);

                        Ray scattered_ray{};
                        glm::vec3 attenuation{};
                        if (scene_isect_record.tri_ptr->get_mat_ptr()->is_scattering(rays[i], scene_isect_record, attenuation, scattered_ray)) {

                            // TODO: Now: calc signal strength using Friss -> should use EM wave propagation model
                            if (method == "friss") {
                                glm::vec3 normal{ scene_isect_record.normal };
                                glm::vec3 incident_dir{ glm::normalize(rays[i].get_direction()) };
                                glm::vec3 reflected_dir{ glm::normalize(glm::reflect(incident_dir, normal)) };

                                float cos_2theta1 = glm::dot(incident_dir, reflected_dir);
                                float incident_angle = std::acos(cos_2theta1) / 2;
                                float ref_coef{ calc_reflection_coefficient(incident_angle, 1.0f, scene_isect_record.tri_ptr->get_mat_ptr()->calc_real_relative_permittivity(tx_freq), polar) };

                                float added_strength{ calc_friss_strength(start_pos, scene_isect_record.point, tx_freq, start_strength, start_gain, end_gain, ref_coef) };

                                tmp_path_recs[i].set_signal_strength(added_strength);
                            }

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

#pragma omp critical
            {
                if (path_recs != nullptr) {
                    for (int i = 0; i < m_num_rays; i++) {
                        if (!tmp_path_recs[i].is_empty()) {
                            (*path_recs).emplace_back(tmp_path_recs[i]);
                        }
                    }
                }
            }

            std::clog << "Coverage map is generated" << std::endl;
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

        Quad make_coverage_quad(const AABB& scene_box, float map_height = 1.5f) {
            glm::vec3 box_min{ scene_box.get_min() };
            glm::vec3 box_max{ scene_box.get_max() };

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
                    path_rec.add_record(isect_rec.point, isect_rec.tri_ptr->get_mat_ptr(), isect_rec.tri_ptr);
                    Ray scattered_ray{};
                    glm::vec3 attenuation{};
                    if (isect_rec.tri_ptr->get_mat_ptr()->is_scattering(cur_ray, isect_rec, attenuation, scattered_ray)) {
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