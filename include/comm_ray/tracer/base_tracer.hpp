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
    // class BaseTracer : public TracerInterface {
    // public:
    //     BaseTracer() = default;

    //     BaseTracer(const std::vector<Model>& models, int max_reflection = 2)
    //         : m_max_reflection{ max_reflection }
    //         , m_triangles{ init_triangles(models) }
    //         , m_bvh{ BVHAccel{ m_triangles, 0, m_triangles.size() } } {}

    //     BaseTracer(const std::vector<std::reference_wrapper<Model>>& models, int max_reflection = 2)
    //         : m_max_reflection{ max_reflection }
    //         , m_triangles{ init_triangles(models) }
    //         , m_bvh{ BVHAccel{ m_triangles, 0, m_triangles.size() } } {}

    //     virtual ~BaseTracer() = default;

    //     //copy constructor
    //     BaseTracer(const BaseTracer& other)
    //         : m_max_reflection{ other.m_max_reflection }
    //         , m_triangles{ other.m_triangles }
    //         , m_bvh{ other.m_bvh } {}

    //     //copy assignment
    //     BaseTracer& operator=(const BaseTracer& other) {
    //         m_max_reflection = other.m_max_reflection;
    //         m_triangles = other.m_triangles;
    //         m_bvh = other.m_bvh;
    //         return *this;
    //     }

    //     // move constructor
    //     BaseTracer(BaseTracer&& other)
    //         : m_max_reflection{ other.m_max_reflection }
    //         , m_triangles{ other.m_triangles }
    //         , m_bvh{ other.m_bvh } {}

    //     // move assignment
    //     BaseTracer& operator=(BaseTracer&& other) noexcept {
    //         m_max_reflection = other.m_max_reflection;
    //         m_triangles = other.m_triangles;
    //         m_bvh = other.m_bvh;
    //         return *this;
    //     }

    //     // void reset() override {};
    //     // void trace_rays(glm::vec3& tx_pos, glm::vec3& rx_pos, std::vector<ReflectionRecord>& ref_records) override;

    // protected:
    //     std::vector<std::shared_ptr<Hittable>> init_triangles(const std::vector<Model>& models) override {
    //         std::vector<std::shared_ptr<Hittable>> triangles{};
    //         std::vector<Vertex> vertex_buffer;
    //         vertex_buffer.reserve(3);
    //         int triangle_count{ 0 };
    //         int vertex_count{ 0 };
    //         for (const auto& model : models) {
    //             for (const auto& mesh : model.get_meshes()) {
    //                 for (const auto& idx : mesh.get_indices()) {
    //                     vertex_count++;
    //                     vertex_buffer.push_back(mesh.get_vertices()[idx]);
    //                     triangle_count++;
    //                     if (triangle_count == 3) {
    //                         triangles.emplace_back(std::make_shared<Triangle>(
    //                             vertex_buffer[0].position,
    //                             vertex_buffer[1].position,
    //                             vertex_buffer[2].position)
    //                         );
    //                         vertex_buffer.clear();
    //                         triangle_count = 0;
    //                     }
    //                 }
    //             }
    //         }
    //         std::cout << "Triangle count: " << triangles.size() << std::endl;
    //         std::cout << "Vertex count: " << vertex_count << std::endl;
    //         return triangles;
    //     };

    //     std::vector<std::shared_ptr<Hittable>> init_triangles(const std::vector<std::reference_wrapper<Model>>& models) override {
    //         std::vector<std::shared_ptr<Hittable>> triangles{};
    //         std::vector<Vertex> vertex_buffer;
    //         vertex_buffer.reserve(3);
    //         int triangle_count{ 0 };
    //         int vertex_count{ 0 };
    //         for (const auto& model : models) {
    //             for (const auto& mesh : model.get().get_meshes()) {
    //                 for (const auto& idx : mesh.get_indices()) {
    //                     vertex_count++;
    //                     vertex_buffer.push_back(mesh.get_vertices()[idx]);
    //                     triangle_count++;
    //                     if (triangle_count == 3) {
    //                         triangles.emplace_back(std::make_shared<Triangle>(
    //                             vertex_buffer[0].position,
    //                             vertex_buffer[1].position,
    //                             vertex_buffer[2].position)
    //                         );
    //                         vertex_buffer.clear();
    //                         triangle_count = 0;
    //                     }
    //                 }
    //             }
    //         }
    //         std::cout << "Triangle count: " << triangles.size() << std::endl;
    //         std::cout << "Vertex count: " << vertex_count << std::endl;
    //         return triangles;
    //     };

    // protected:
    //     int m_max_reflection{ 2 };
    //     std::vector<std::shared_ptr<Hittable>> m_triangles{};
    //     BVHAccel m_bvh;
    // };

#ifdef BVH1

    class BaseTracer : public TracerInterface {
    public:
        BaseTracer() = default;

        BaseTracer(const std::vector<Model>& models, int max_reflection = 2)
            : m_max_reflection{ max_reflection } {
            for (std::size_t i = 0; i < models.size(); ++i) {
                auto triangles{ init_triangles(models[i]) };
                m_bvhs.emplace_back(BVHAccel{ triangles, 0, triangles.size() });
            }
            // m_tlas = TLAS{ m_bvhs, m_bvhs.size() };
            // m_tlas.build();
        }

        BaseTracer(const std::vector<std::reference_wrapper<Model>>& models, int max_reflection = 2)
            : m_max_reflection{ max_reflection } {
            std::vector<std::shared_ptr<Hittable>> triangles{};
            for (std::size_t i = 0; i < models.size(); ++i) {
                auto tmp_triangles{ init_triangles(models[i].get()) };
                triangles.insert(triangles.end(), tmp_triangles.begin(), tmp_triangles.end());
            }
            std::cout << "Triangle count: " << triangles.size() << std::endl;
            m_bvhs.emplace_back(BVHAccel{ triangles, 0, triangles.size() });
            // m_tlas = TLAS{ m_bvhs, m_bvhs.size() };
            // m_tlas.build();
        }

        virtual ~BaseTracer() = default;

        //copy constructor
        BaseTracer(const BaseTracer& other)
            : m_max_reflection{ other.m_max_reflection }
            , m_triangles{ other.m_triangles }
            , m_bvhs{ other.m_bvhs }
            // , m_tlas{ other.m_tlas } 
        {}

        //copy assignment
        BaseTracer& operator=(const BaseTracer& other) {
            m_max_reflection = other.m_max_reflection;
            m_triangles = other.m_triangles;
            m_bvhs = other.m_bvhs;
            // m_tlas = other.m_tlas;
            return *this;
        }

        // move constructor
        BaseTracer(BaseTracer&& other)
            : m_max_reflection{ other.m_max_reflection }
            , m_triangles{ other.m_triangles }
            , m_bvhs{ other.m_bvhs }
            // , m_tlas{ other.m_tlas } 
        {}

        // move assignment
        BaseTracer& operator=(BaseTracer&& other) noexcept {
            m_max_reflection = other.m_max_reflection;
            m_triangles = other.m_triangles;
            m_bvhs = other.m_bvhs;
            // m_tlas = other.m_tlas;
            return *this;
        }

        // void reset() override {};
        // void trace_rays(glm::vec3& tx_pos, glm::vec3& rx_pos, std::vector<ReflectionRecord>& ref_records) override;

    protected:
        std::vector<std::shared_ptr<Hittable>> init_triangles(const Model& model) override {
            std::vector<std::shared_ptr<Hittable>> triangles{};
            std::vector<Vertex> vertex_buffer;
            vertex_buffer.reserve(3);
            int triangle_count{ 0 };
            int vertex_count{ 0 };
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
            std::cout << "Triangle count: " << triangles.size() << std::endl;
            std::cout << "Vertex count: " << vertex_count << std::endl;
            return triangles;
        };

        std::vector<std::shared_ptr<Hittable>> init_triangles(const std::reference_wrapper<Model>& model) override {
            std::vector<std::shared_ptr<Hittable>> triangles{};
            std::vector<Vertex> vertex_buffer;
            vertex_buffer.reserve(3);
            int triangle_count{ 0 };
            int vertex_count{ 0 };
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
            std::cout << "Triangle count: " << triangles.size() << std::endl;
            std::cout << "Vertex count: " << vertex_count << std::endl;
            return triangles;
        };

        std::vector<std::shared_ptr<Hittable>> init_triangles(const std::vector<Model>& models) override {
            std::vector<std::shared_ptr<Hittable>> triangles{};
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

        std::vector<std::shared_ptr<Hittable>> init_triangles(const std::vector<std::reference_wrapper<Model>>& models) override {
            std::vector<std::shared_ptr<Hittable>> triangles{};
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
        std::vector<std::shared_ptr<Hittable>> m_triangles{};
        std::vector<BVHAccel> m_bvhs{};
        // TLAS m_tlas{};
    };

#elif defined(BVH3)

    class BaseTracer : public TracerInterface {
    public:
        BaseTracer() = default;

        BaseTracer(const std::vector<Model>& models, int max_reflection = 2)
            : m_max_reflection{ max_reflection } {
            m_bvhs.reserve(models.size());
            glm::mat4 tmp_trans2{ glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 0.0f, 60.0f, 0.0f }) * glm::rotate(glm::mat4{ 1.0f }, glm::radians(180.0f), glm::vec3{ 1.0f, 0.0f, 0.0f }) };

            for (std::size_t i = 0; i < models.size(); ++i) {
                std::shared_ptr<BVHAccel> bvh_ptr{ std::make_shared<BVHAccel>(models[i]) };
                for (int j = 0; j < 2; ++j) {
                    glm::mat4 trans{ glm::mat4(1.0f) };
                    if (j == 1) { trans = tmp_trans2; }
                    else { trans = glm::mat4(1.0f); }
                    BVHInstance bvh_inst{ bvh_ptr, trans };
                    m_bvhs.emplace_back(bvh_inst);
                }
            }
            m_tlas = TLAS{ m_bvhs, static_cast<uint>(m_bvhs.size()) };
            m_tlas.build();
        }

        BaseTracer(const std::vector<std::reference_wrapper<Model>>& models, int max_reflection = 2)
            : m_max_reflection{ max_reflection } {
            m_bvhs.reserve(models.size() * 16);
            glm::mat4 tmp_trans2{ glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 0.0f, 60.0f, 0.0f }) * glm::rotate(glm::mat4{ 1.0f }, glm::radians(180.0f), glm::vec3{ 1.0f, 0.0f, 0.0f }) };

            for (std::size_t i = 0; i < models.size(); ++i) {
                std::shared_ptr<BVHAccel> bvh_ptr{ std::make_shared<BVHAccel>(models[i].get()) };
                for (int j = 0; j < 2; ++j) {
                    glm::mat4 trans{ glm::mat4(1.0f) };
                    if (j == 1) { trans = tmp_trans2; }
                    else { trans = glm::mat4(1.0f); }
                    BVHInstance bvh_inst{ bvh_ptr, trans };
                    m_bvhs.emplace_back(bvh_inst);
                }
            }
            m_tlas = TLAS{ m_bvhs, static_cast<uint>(m_bvhs.size()) };
            m_tlas.build();
        }

        virtual ~BaseTracer() = default;

        //copy constructor
        BaseTracer(const BaseTracer& other)
            : m_max_reflection{ other.m_max_reflection }
            , m_bvhs{ other.m_bvhs }
            , m_tlas{ other.m_tlas } {}

        //copy assignment
        BaseTracer& operator=(const BaseTracer& other) {
            m_max_reflection = other.m_max_reflection;
            m_bvhs = other.m_bvhs;
            m_tlas = other.m_tlas;
            return *this;
        }

        // move constructor
        BaseTracer(BaseTracer&& other)
            : m_max_reflection{ other.m_max_reflection }
            , m_bvhs{ other.m_bvhs }
            , m_tlas{ other.m_tlas } {}

        // move assignment
        BaseTracer& operator=(BaseTracer&& other) noexcept {
            m_max_reflection = other.m_max_reflection;
            m_bvhs = other.m_bvhs;
            m_tlas = other.m_tlas;
            return *this;
        }

        // void reset() override {};
        // void trace_rays(glm::vec3& tx_pos, glm::vec3& rx_pos, std::vector<ReflectionRecord>& ref_records) override;

    protected:
        // std::vector<std::shared_ptr<Hittable>> init_triangles(const Model& model) override {
        //     std::vector<std::shared_ptr<Hittable>> triangles{};
        //     std::vector<Vertex> vertex_buffer;
        //     vertex_buffer.reserve(3);
        //     int triangle_count{ 0 };
        //     int vertex_count{ 0 };
        //     for (const auto& mesh : model.get_meshes()) {
        //         for (const auto& idx : mesh.get_indices()) {
        //             vertex_count++;
        //             vertex_buffer.push_back(mesh.get_vertices()[idx]);
        //             triangle_count++;
        //             if (triangle_count == 3) {
        //                 triangles.emplace_back(std::make_shared<Triangle>(
        //                     vertex_buffer[0].position,
        //                     vertex_buffer[1].position,
        //                     vertex_buffer[2].position)
        //                 );
        //                 vertex_buffer.clear();
        //                 triangle_count = 0;
        //             }
        //         }
        //     }
        //     std::cout << "Triangle count: " << triangles.size() << std::endl;
        //     std::cout << "Vertex count: " << vertex_count << std::endl;
        //     return triangles;
        // };

        // std::vector<std::shared_ptr<Hittable>> init_triangles(const std::reference_wrapper<Model>& model) override {
        //     std::vector<std::shared_ptr<Hittable>> triangles{};
        //     std::vector<Vertex> vertex_buffer;
        //     vertex_buffer.reserve(3);
        //     int triangle_count{ 0 };
        //     int vertex_count{ 0 };
        //     for (const auto& mesh : model.get().get_meshes()) {
        //         for (const auto& idx : mesh.get_indices()) {
        //             vertex_count++;
        //             vertex_buffer.push_back(mesh.get_vertices()[idx]);
        //             triangle_count++;
        //             if (triangle_count == 3) {
        //                 triangles.emplace_back(std::make_shared<Triangle>(
        //                     vertex_buffer[0].position,
        //                     vertex_buffer[1].position,
        //                     vertex_buffer[2].position)
        //                 );
        //                 vertex_buffer.clear();
        //                 triangle_count = 0;
        //             }
        //         }
        //     }
        //     std::cout << "Triangle count: " << triangles.size() << std::endl;
        //     std::cout << "Vertex count: " << vertex_count << std::endl;
        //     return triangles;
        // };

        // std::vector<std::shared_ptr<Hittable>> init_triangles(const std::vector<Model>& models) override {
        //     std::vector<std::shared_ptr<Hittable>> triangles{};
        //     std::vector<Vertex> vertex_buffer;
        //     vertex_buffer.reserve(3);
        //     int triangle_count{ 0 };
        //     int vertex_count{ 0 };
        //     for (const auto& model : models) {
        //         for (const auto& mesh : model.get_meshes()) {
        //             for (const auto& idx : mesh.get_indices()) {
        //                 vertex_count++;
        //                 vertex_buffer.push_back(mesh.get_vertices()[idx]);
        //                 triangle_count++;
        //                 if (triangle_count == 3) {
        //                     triangles.emplace_back(std::make_shared<Triangle>(
        //                         vertex_buffer[0].position,
        //                         vertex_buffer[1].position,
        //                         vertex_buffer[2].position)
        //                     );
        //                     vertex_buffer.clear();
        //                     triangle_count = 0;
        //                 }
        //             }
        //         }
        //     }
        //     std::cout << "Triangle count: " << triangles.size() << std::endl;
        //     std::cout << "Vertex count: " << vertex_count << std::endl;
        //     return triangles;
        // };

        // std::vector<std::shared_ptr<Hittable>> init_triangles(const std::vector<std::reference_wrapper<Model>>& models) override {
    //     std::vector<std::shared_ptr<Hittable>> triangles{};
    //     std::vector<Vertex> vertex_buffer;
    //     vertex_buffer.reserve(3);
    //     int triangle_count{ 0 };
    //     int vertex_count{ 0 };
    //     for (const auto& model : models) {
    //         for (const auto& mesh : model.get().get_meshes()) {
    //             for (const auto& idx : mesh.get_indices()) {
    //                 vertex_count++;
    //                 vertex_buffer.push_back(mesh.get_vertices()[idx]);
    //                 triangle_count++;
    //                 if (triangle_count == 3) {
    //                     triangles.emplace_back(std::make_shared<Triangle>(
    //                         vertex_buffer[0].position,
    //                         vertex_buffer[1].position,
    //                         vertex_buffer[2].position)
    //                     );
    //                     vertex_buffer.clear();
    //                     triangle_count = 0;
    //                 }
    //             }
    //         }
    //     }
    //     std::cout << "Triangle count: " << triangles.size() << std::endl;
    //     std::cout << "Vertex count: " << vertex_count << std::endl;
    //     return triangles;
    // };

    protected:
        int m_max_reflection{ 2 };
        std::vector<BVHInstance> m_bvhs{};
        TLAS m_tlas{};
    };

#endif
}



#endif //! _COMM_RAY_TRACER_BASE_TRACER_HPP_