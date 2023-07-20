#pragma once

#ifndef _COMM_RAY_TRACER_BASE_TRACER_HPP_
#define _COMM_RAY_TRACER_BASE_TRACER_HPP_

#include "reflection_record.hpp"
#include "model.hpp"
#include "triangle.hpp"
#include "glm/glm.hpp"
#include "bvh_map.hpp"
#include "tracer_interface.hpp"

namespace SignalTracer {
    class BaseTracer : public TracerInterface {
    public:
        BaseTracer() = default;

        BaseTracer(const std::vector<Model>& models, int max_reflection = 2)
            : m_max_reflection{ max_reflection }
            , m_triangles{ init_triangles(models) }
            , m_bvh{ BVH{ m_triangles, 0, m_triangles.size() } } {}

        BaseTracer(const std::vector<std::reference_wrapper<Model>>& models, int max_reflection = 2)
            : m_max_reflection{ max_reflection }
            , m_triangles{ init_triangles(models) }
            , m_bvh{ BVH{ m_triangles, 0, m_triangles.size() } } {}

        virtual ~BaseTracer() = default;

        //copy constructor
        BaseTracer(const BaseTracer& other)
            : m_max_reflection{ other.m_max_reflection }
            , m_triangles{ other.m_triangles }
            , m_bvh{ other.m_bvh } {}

        //copy assignment
        BaseTracer& operator=(const BaseTracer& other) {
            m_max_reflection = other.m_max_reflection;
            m_triangles = other.m_triangles;
            m_bvh = other.m_bvh;
            return *this;
        }

        // move constructor
        BaseTracer(BaseTracer&& other)
            : m_max_reflection{ other.m_max_reflection }
            , m_triangles{ other.m_triangles }
            , m_bvh{ other.m_bvh } {}

        // move assignment
        BaseTracer& operator=(BaseTracer&& other) noexcept {
            m_max_reflection = other.m_max_reflection;
            m_triangles = other.m_triangles;
            m_bvh = other.m_bvh;
            return *this;
        }

        // void reset() override {};
        // void trace_rays(glm::vec3& tx_pos, glm::vec3& rx_pos, std::vector<ReflectionRecord>& ref_records) override;

    protected:
        std::vector<std::shared_ptr<Triangle>> init_triangles(const std::vector<Model>& models) override {
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
        };

        std::vector<std::shared_ptr<Triangle>> init_triangles(const std::vector<std::reference_wrapper<Model>>& models) override {
            std::vector<std::shared_ptr<Triangle>> triangles{};
            std::vector<Vertex> vertex_buffer;
            vertex_buffer.reserve(3);
            int triangle_count{ 0 };
            int vertex_count{ 0 };
            for (const auto& model : models) {
                for (const auto& mesh : model.get().get_meshes()) {
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
        };

    protected:
        int m_max_reflection{ 2 };
        std::vector<std::shared_ptr<Triangle>> m_triangles{};
        BVH m_bvh{};
    };
}



#endif //! _COMM_RAY_TRACER_BASE_TRACER_HPP_