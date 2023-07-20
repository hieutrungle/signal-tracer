#pragma once

#ifndef RAY_CASTING_TRACER_HPP
#define RAY_CASTING_TRACER_HPP


#include "base_tracer.hpp"
#include "model.hpp"
#include "bvh_map.hpp"
#include "constant.hpp"
#include "reflection_record.hpp"
#include "triangle.hpp"
#include "utils.hpp"
#include "material.hpp"
#include "intersect_record.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

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

        void trace_rays(glm::vec3& tx_pos, glm::vec3& rx_pos, std::vector<ReflectionRecord>& ref_records) override {
            reset();
            std::clog << "tx position: " << glm::to_string(tx_pos) << std::endl;
            std::clog << "rx position: " << glm::to_string(rx_pos) << std::endl;
            glm::vec3 up{ 0.0f, 1.0f, 0.0f };
            glm::vec3 right{ 1.0f, 0.0f, 0.0f };
            for (float i = 0; i < 360; i += m_angular_interval) {
                for (float j = 0; j < 360; j += m_angular_interval) {
                    glm::vec3 direction = spherical2cartesian(i, j);
                    Ray ray{ tx_pos, direction };
                    ReflectionRecord ref_record{};
                    ref_record.add_point(tx_pos);
                    trace_ray(ray, rx_pos, m_rx_radius, m_max_reflection, ref_record);
                    if (!ref_record.is_empty()) {
                        ref_records.emplace_back(ref_record);
                    }
                }
            }
        };

        void trace_ray(const Ray& ray, const glm::vec3& rx_pos, const float& rx_radius, int depth, ReflectionRecord& ref_record) {

            if (depth < 0) {
                ref_record.clear();
                return;
            }
            IntersectRecord record{};
            Interval interval{ Constant::EPSILON, Constant::INFINITY_POS };
            glm::vec3 projection_point{};

            // Ray: Origin + t * Direction
            // t0: the distance between Origin and the projection point on the ray
            // t0 = (Origin - Rx_pos) * Direction
            float t0 = glm::dot(ray.get_direction(), rx_pos - ray.get_origin());

            // save new IntersectionRecord to record
            bool b_is_hit = m_bvh.is_hit(ray, interval, record);

            if (t0 >= 0 && t0 <= record.get_t()) {
                projection_point = ray.get_origin() + t0 * ray.get_direction();
                float rx_projection_len = glm::length(projection_point - rx_pos);

                if (rx_projection_len <= rx_radius) {
                    ref_record.add_record(projection_point);
                    return;
                }
            }

            if (b_is_hit) {
                ref_record.add_record(record.get_point(), record.get_material_ptr());
                Ray scattered_ray{};
                glm::vec3 attenuation{};
                if (record.get_material_ptr()->is_scattering(ray, record, attenuation, scattered_ray)) {
                    trace_ray(scattered_ray, rx_pos, rx_radius, depth - 1, ref_record);
                    return;
                }
            }

            // ray does not hit any object and does not reach the receiver
            ref_record.clear();
            return;
        }

        glm::vec3 spherical2cartesian(float yaw, float pitch) {
            glm::vec3 cartesian;
            cartesian.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            cartesian.y = sin(glm::radians(pitch));
            cartesian.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            return cartesian;
        }

    private:
        // Low resolution: 1.0 degree
        // Medium resolution: 0.5 degree
        // High resolution: 0.25 degree
        float m_angular_interval{ 0.5f };
        float m_rx_radius{ 0.1f };
    };
}


#endif // !RAY_CASTING_TRACER_HPP