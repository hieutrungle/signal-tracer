#pragma once

#ifndef RAY_CASTING_TRACER_HPP
#define RAY_CASTING_TRACER_HPP


#include "base_tracer.hpp"
#include "bvh_map.hpp"
#include "constant.hpp"
#include "intersect_record.hpp"
#include "path_record.hpp"
#include "triangle.hpp"
#include "utils.hpp"
#include "material.hpp"
#include "model.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include <functional>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>

namespace SignalTracer {

    class RayCastingTracer : public BaseTracer {
    public:
        RayCastingTracer() = default;

        RayCastingTracer(const std::vector<Model>& models, int max_reflection = 2)
            : BaseTracer{ models, max_reflection } {}

        RayCastingTracer(const std::vector<std::reference_wrapper<Model>>& models, int max_reflection = 2)
            : BaseTracer{ models, max_reflection } {}

        RayCastingTracer(std::vector<std::reference_wrapper<Model>>& models, int max_reflection = 2, const float& angular_interval = 0.5f)
            : BaseTracer{ models, max_reflection }, m_angular_interval{ angular_interval } {
            m_rx_radius = (m_angular_interval < 1.0f) ? m_angular_interval / 2.0f : 1.0f / 2.0f;
        }

        ~RayCastingTracer() override = default;

        // copy constructor
        RayCastingTracer(const RayCastingTracer& other)
            : BaseTracer{ other } {}

        // copy assignment
        RayCastingTracer& operator=(const RayCastingTracer& other) {
            BaseTracer::operator=(other);
            return *this;
        }

        // move constructor
        RayCastingTracer(RayCastingTracer&& other)
            : BaseTracer{ std::move(other) } {}

        // move assignment
        RayCastingTracer& operator=(RayCastingTracer&& other) noexcept {
            BaseTracer::operator=(std::move(other));
            return *this;
        }

        void reset() override {}

        void trace_rays(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<PathRecord>& ref_records) override {
            // trace_rays_sequential_fibo(tx_pos, rx_pos, ref_records);
            trace_rays_parallel_fibo(tx_pos, rx_pos, ref_records);
        };

        void trace_rays_sequential(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<PathRecord>& ref_records) {
            reset();
            std::clog << "Running in sequential mode" << std::endl;
            std::clog << "tx position: " << glm::to_string(tx_pos) << std::endl;
            std::clog << "rx position: " << glm::to_string(rx_pos) << std::endl;
            glm::vec3 up{ 0.0f, 1.0f, 0.0f };
            glm::vec3 right{ 1.0f, 0.0f, 0.0f };
            for (float i = 0; i < 360; i += m_angular_interval) {
                trace_vertial_rays(tx_pos, rx_pos, ref_records, i);
            }
        }

        void trace_rays_parallel(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<PathRecord>& ref_records) {
            reset();
            std::clog << "Running in parallel mode" << std::endl;
            std::clog << "tx position: " << glm::to_string(tx_pos) << std::endl;
            std::clog << "rx position: " << glm::to_string(rx_pos) << std::endl;
            glm::vec3 up{ 0.0f, 1.0f, 0.0f };
            glm::vec3 right{ 1.0f, 0.0f, 0.0f };
            const int num_threads{ static_cast<int>(round(360.0f / m_angular_interval)) + 1 };
            std::vector<std::thread> threads{};
            std::vector<std::vector<PathRecord>> ref_records_vec(num_threads);

            // horizontal -> vertical
            for (float i = 0.0; i < 360.0; i += m_angular_interval) {
                threads.emplace_back(std::thread(&RayCastingTracer::trace_vertial_rays, this, tx_pos, rx_pos, std::ref(ref_records_vec[round(i / m_angular_interval)]), i));
            }

            // vertical -> horizontal
            // for (float j = 180.0; j < 360.0; j += m_angular_interval) {
            //     threads.emplace_back(std::thread(&RayCastingTracer::trace_horizontal_rays, this, tx_pos, rx_pos, std::ref(ref_records_vec[round(j / m_angular_interval)]), j));
            // }

            for (auto& thread : threads) {
                if (thread.joinable()) {
                    thread.join();
                }
            }

            for (auto& tmp_ref_records : ref_records_vec) {
                std::copy_if(tmp_ref_records.begin(), tmp_ref_records.end(), std::back_inserter(ref_records), [](const PathRecord& path_rec) {
                    return !path_rec.is_empty();
                    });
            }
        };

        void trace_rays_sequential_fibo(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<PathRecord>& ref_records) {
            reset();
            std::clog << "Running in sequential mode" << std::endl;
            std::clog << "tx position: " << glm::to_string(tx_pos) << std::endl;
            std::clog << "rx position: " << glm::to_string(rx_pos) << std::endl;
            glm::vec3 up{ 0.0f, 1.0f, 0.0f };
            glm::vec3 right{ 1.0f, 0.0f, 0.0f };
            int num_rays{ static_cast<int>(360.0f * 360.0f / m_angular_interval / m_angular_interval) };
            std::vector <glm::vec3> directions{ Utils::get_fibonacci_lattice(num_rays) };

            const int num_threads{ static_cast<int>(directions.size()) };
            std::vector<PathRecord> ref_records_vec(num_threads);

            for (int i = 0; i < num_threads; i++) {
                const Ray ray{ tx_pos, directions[i] };
                trace_fibonacci_ray(tx_pos, rx_pos, ref_records_vec[i], ray);
            }

            std::copy_if(ref_records_vec.begin(), ref_records_vec.end(), std::back_inserter(ref_records), [](const PathRecord& path_rec) {
                return !path_rec.is_empty();
                });

        }

        // Tracing using Fibonacci lattice
        void trace_rays_parallel_fibo(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<PathRecord>& ref_records) {
            reset();
            std::clog << "Running in parallel mode" << std::endl;
            std::clog << "tx position: " << glm::to_string(tx_pos) << std::endl;
            std::clog << "rx position: " << glm::to_string(rx_pos) << std::endl;
            glm::vec3 up{ 0.0f, 1.0f, 0.0f };
            glm::vec3 right{ 1.0f, 0.0f, 0.0f };
            int num_rays{ int(6e6) };

            Utils::Timer timer{};
            std::vector <glm::vec3> directions{ Utils::get_fibonacci_lattice(num_rays) };
            timer.execution_time();

            timer.reset();

            int num_threads{ int(std::thread::hardware_concurrency()) * 10 };
            int num_rays_per_thread{ num_rays / num_threads };
            std::vector<std::thread> threads(num_threads);
            std::vector<std::vector<PathRecord>> ref_records_vec(num_threads);

            for (int i = 0; i < num_threads;i++) {
                std::vector<Ray> rays(num_rays_per_thread);
                for (int j = i * num_rays_per_thread; j < (i + 1) * num_rays_per_thread; j++) {
                    rays[j - i * num_rays_per_thread] = Ray{ tx_pos, directions[j] };
                }
                threads.emplace_back(std::thread(&RayCastingTracer::trace_fibonacci_rays, this, tx_pos, rx_pos, std::ref(ref_records_vec[i]), rays));
            }

            for (auto& thread : threads) {
                if (thread.joinable()) {
                    thread.join();
                }
            }

            for (auto& tmp_ref_records : ref_records_vec) {
                std::copy_if(tmp_ref_records.begin(), tmp_ref_records.end(), std::back_inserter(ref_records), [](const PathRecord& path_rec) {
                    return !path_rec.is_empty();
                    });
            }

            timer.execution_time();
        };

    private:
        void trace_horizontal_rays(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<PathRecord>& ref_records, const float& pitch_angle) {
            for (float i = 0.0; i < 360.0; i += m_angular_interval) {
                glm::vec3 direction = Utils::spherical2cartesian(i, pitch_angle);
                Ray ray{ tx_pos, direction };
                PathRecord path_rec{};
                path_rec.add_point(tx_pos);
                trace_ray(ray, rx_pos, m_rx_radius, m_max_reflection, path_rec);
                if (!path_rec.is_empty()) {
                    ref_records.emplace_back(path_rec);
                }
            }
        }

        // function for concurrency, thread
        void trace_vertial_rays(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<PathRecord>& ref_records, const float& yaw_angle) {
            for (float j = 180; j < 360; j += m_angular_interval) {
                glm::vec3 direction = Utils::spherical2cartesian(yaw_angle, j);
                Ray ray{ tx_pos, direction };
                PathRecord path_rec{};
                path_rec.add_point(tx_pos);
                trace_ray(ray, rx_pos, m_rx_radius, m_max_reflection, path_rec);
                if (!path_rec.is_empty()) {
                    ref_records.emplace_back(path_rec);
                }
            }
        }

        void trace_fibonacci_rays(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<PathRecord>& ref_records, const std::vector<Ray>& ray) {
            for (const auto& r : ray) {
                PathRecord path_rec{};
                path_rec.add_point(tx_pos);
                trace_ray(r, rx_pos, m_rx_radius, m_max_reflection, path_rec);
                if (!path_rec.is_empty()) {
                    ref_records.emplace_back(path_rec);
                }
            }
        }

        void trace_fibonacci_ray(const glm::vec3& tx_pos, const glm::vec3& rx_pos, PathRecord& path_rec, const Ray& ray) {
            path_rec.add_point(tx_pos);
            trace_ray(ray, rx_pos, m_rx_radius, m_max_reflection, path_rec);
        }

        void trace_ray(const Ray& ray, const glm::vec3& rx_pos, const float& rx_radius, int depth, PathRecord& path_rec) {
            if (depth < 0) {
                path_rec.clear();
                return;
            }
            IntersectRecord record{};
            Interval interval{ Constant::EPSILON, Constant::INF_POS };
            glm::vec3 projection_point{};

            // Ray: Origin + t * Direction
            // t0: the distance between Origin and the projection point on the ray
            // t0 = (Origin - Rx_pos) * Direction
            float t0 = glm::dot(ray.get_direction(), rx_pos - ray.get_origin());

            // save new IntersectionRecord to record
            bool b_is_hit{ m_tlas.is_hit(ray, interval, record) };

            if (t0 >= 0 && t0 <= record.t) {
                // if (t0 >= 0 && t0 <= record.get_t()) {
                projection_point = ray.get_origin() + t0 * ray.get_direction();
                float rx_projection_len = glm::length(projection_point - rx_pos);

                if (rx_projection_len <= rx_radius) {
                    path_rec.add_record(projection_point);
                    return;
                }
            }

            // if (b_is_hit) {
            //     path_rec.add_record(record.get_point(), record.get_material_ptr(), record.get_triangle_ptr());
            //     Ray scattered_ray{};
            //     glm::vec3 attenuation{};
            //     if (record.get_material_ptr()->is_scattering(ray, record, attenuation, scattered_ray)) {
            //         trace_ray(scattered_ray, rx_pos, rx_radius, depth - 1, path_rec);
            //         return;
            //     }
            // }

            if (b_is_hit) {
                path_rec.add_record(record.point, record.mat_ptr, record.tri_ptr);
                Ray scattered_ray{};
                glm::vec3 attenuation{};
                if (record.mat_ptr->is_scattering(ray, record, attenuation, scattered_ray)) {
                    trace_ray(scattered_ray, rx_pos, rx_radius, depth - 1, path_rec);
                    return;
                }
            }

            // ray does not hit any object and does not reach the receiver
            path_rec.clear();
            return;
        }

        float m_angular_interval{ 0.5f };
        float m_rx_radius{ 0.1f };
    };

}


#endif // !RAY_CASTING_TRACER_HPP