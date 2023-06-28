#pragma once

#ifndef SIGNAL_TRACER_HPP
#define SIGNAL_TRACER_HPP

#include "model.hpp"
// #include "mesh.hpp"
#include "bvh_map.hpp"
#include "constant.hpp"
#include "cubesphere.h"
#include "line.hpp"
#include "reflection_record.hpp"
#include "triangle.hpp"
#include "point_container.hpp"
#include "glm/glm.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>


// // TODO: Setup world that contains all the objects' triangles similar to the HittableList class in Ray Tracing in One Weekend. This HittableList is a wrapper of all meshes from all models. The BVHMap is a wrapper of all triangles from all meshes. The BVHMap is used to accelerate the intersection test between a ray and the triangles. The HittableList is used to find the closest intersection point between a ray and the triangles.

// // TODO: Setup a world.


/*
    HittableList model{};
    auto material = make_shared<Material>(attributes);
    model.add(make_shared<Triangle>(p1, p2, p3, material));

    ------------------------------------------
    BVH constructor
    BVH(const std::vector<shared_ptr<Hittable>>& src_objects, size_t start, size_t end) { init BVH structure }
    BVH(const HittableList& model) : BVH{ model.objects(), 0, model.objects().size() } {}

    There are two ways to initialize a BVH:
    1. Initialize a BVH with a HittableList object (a model).
    2. Initialize a BVH with a vector of shared_ptr<Hittable> objects (shared pointers of all triangles).

    we choose the first method. Initialize a model object with all triangles from all meshes. Then initialize a BVH with the model object.
    ------------------------------------------

    std::shared_ptr<BVH> bvh = std::make_shared<BVH>(model);

    ------------------------------------------
    There can be many model we want to render. We can store all models in a vector that is a member of the world object (a bigger HittableList).
    HittableList member variable:
    std::vector<shared_ptr<Hittable>> m_objects{};

    Child class of Hittable:
        class BVH : public
        class Triangle : public
        class HittableList : public
    ------------------------------------------

    HittableList model1{};
    auto material1 = make_shared<Material>(attributes1);
    model1.add(make_shared<Triangle>(p11, p12, p13, material1));

    HittableList model2{};
    auto material2 = make_shared<Material>(attributes2);
    model2.add(make_shared<Triangle>(p21, p22, p23, material2));

    std::shared_ptr<BVH> bvh1 = std::make_shared<BVH>(model1);
    std::shared_ptr<BVH> bvh2 = std::make_shared<BVH>(model2);

    std::vector<std::shared_ptr<Hittable>> models{};
    models.push_back(bvh1);
    models.push_back(bvh2);

    HittalbeList world{};
    world.add(models);

    world.intersect(ray, interval, record);

    ------------------------------------------
    When calling world.intersect(), the world object will call the intersect() function of each model object. The model object, which is a BVH, will perform the intersection test between the ray and and its AABB.
    If the ray intersects with the AABB, the BVH object will call the intersect() function of its left and right child nodes. The left and right child nodes are either BVH objects or Triangle objects (leaf nodes are triangles).
    If the traversal arrives at a leaf node (a triangle) of the BVH structure, that Triangle object will call its intersect() function to record the intersection information to the IntersectRecord object.
    ------------------------------------------
*/

/*
    // TODO: Implement reflective paths based on mirror reflection.
*/


namespace signal_tracer {

    enum class StationColor {
        red,
        green,
        blue,
        yellow,
        cyan,
        magenta,
        orange,
        purple,
        pink,
        brown,
    };

    constexpr auto operator+(StationColor color) noexcept {
        return static_cast<std::underlying_type_t<StationColor>>(color);
    }

    const glm::vec3 get_station_color(StationColor color) {
        switch (color) {
        case StationColor::red:
            return Constant::RED;
        case StationColor::green:
            return Constant::GREEN;
        case StationColor::blue:
            return Constant::BLUE;
        case StationColor::yellow:
            return Constant::YELLOW;
        case StationColor::cyan:
            return Constant::CYAN;
        case StationColor::magenta:
            return Constant::MAGENTA;
        case StationColor::orange:
            return Constant::ORANGE;
        case StationColor::purple:
            return Constant::PURPLE;
        case StationColor::pink:
            return Constant::PINK;
        case StationColor::brown:
            return Constant::BROWN;
        default:
            return Constant::DARK_GRAY;
        }
    }

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
            : m_triangles{ init_triangles(models) }
            , m_bvh{ std::make_shared<BVH>(m_triangles, 0, m_triangles.size()) }
            , m_max_reflection{ max_reflection } {}

        const std::shared_ptr<BVH>& bvh() const { return m_bvh; }
        int max_reflection() const { return m_max_reflection; }
        const std::vector<ReflectionRecord>& get_reflection_records() const { return m_ref_records; }

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
            m_ref_records = signal_tracer.m_ref_records;
            m_is_direct_lighting = signal_tracer.m_is_direct_lighting;
            m_station_positions = signal_tracer.m_station_positions;
            m_radio_object = signal_tracer.m_radio_object;
            m_lines = signal_tracer.m_lines;
            m_display_reflection_count = signal_tracer.m_display_reflection_count;
            return *this;
        }

        // Move assignment operator
        SignalTracer& operator=(SignalTracer&& signal_tracer) noexcept {
            m_triangles = std::move(signal_tracer.m_triangles);
            m_bvh = std::move(signal_tracer.m_bvh);
            m_max_reflection = std::move(signal_tracer.m_max_reflection);
            m_ref_records = std::move(signal_tracer.m_ref_records);
            m_is_direct_lighting = std::move(signal_tracer.m_is_direct_lighting);
            m_station_positions = std::move(signal_tracer.m_station_positions);
            m_radio_object = std::move(signal_tracer.m_radio_object);
            m_lines = std::move(signal_tracer.m_lines);
            m_display_reflection_count = std::move(signal_tracer.m_display_reflection_count);
            return *this;
        }

        void reset() {
            m_ref_records.clear();
            m_lines.clear();
            m_is_direct_lighting = false;
        }

        void set_display_reflection_count(int reflection_count) {
            m_display_reflection_count = reflection_count;
        }

        void set_positions(const glm::vec3& tx_pos, const glm::vec3& rx_pos) {
            m_station_positions.clear();
            m_station_positions.emplace_back(tx_pos);
            m_station_positions.emplace_back(rx_pos);
        }

        void tracing(glm::vec3 tx_pos, glm::vec3 rx_pos) {
            m_ref_records.clear();
            m_lines.clear();
            m_is_direct_lighting = false;
            set_positions(tx_pos, rx_pos);
            std::clog << "tx position: " << glm::to_string(tx_pos) << std::endl;
            std::clog << "rx position: " << glm::to_string(rx_pos) << std::endl;

            ReflectionRecord ref_record{ 0, std::vector<glm::vec3>{tx_pos} };
            if (trace_direct(tx_pos, rx_pos, ref_record)) {
                m_ref_records.emplace_back(ref_record);
            }
            if (trace_single_reflect(tx_pos, rx_pos, m_ref_records)) {}
            if (trace_double_reflect(tx_pos, rx_pos, m_ref_records)) {}
        }

        bool trace_direct(const glm::vec3& tx_pos, const glm::vec3& rx_pos, ReflectionRecord& ref_record) const {
            Ray ray(tx_pos, rx_pos - tx_pos);
            Interval interval{ Constant::EPSILON, glm::length(rx_pos - tx_pos) - Constant::EPSILON };
            IntersectRecord record{};
            if (!m_bvh->intersect(ray, interval, record)) {
                ref_record.ref_points.emplace_back(rx_pos);
                return true;
            }
            return false;
        }

        bool trace_single_reflect(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<ReflectionRecord>& ref_records) const {
            bool is_reflect = false;
            for (const auto& triangle : m_triangles) {
                glm::vec3 mirror_point = triangle->get_mirror_point(tx_pos);
                Ray ray{ mirror_point, rx_pos - mirror_point };
                Interval interval{ Constant::EPSILON, glm::length(rx_pos - mirror_point) - Constant::EPSILON };
                IntersectRecord record{};
                if (triangle->intersect(ray, interval, record)) {
                    glm::vec3 reflective_point = record.get_point();

                    ReflectionRecord ref_record{ 1, std::vector<glm::vec3>{tx_pos} };
                    if (trace_direct(tx_pos, reflective_point, ref_record) && trace_direct(reflective_point, rx_pos, ref_record)) {
                        ref_records.emplace_back(ref_record);
                        is_reflect = true;
                    }
                }
            }
            return is_reflect;
        }

        bool trace_double_reflect(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<ReflectionRecord>& ref_records) const {
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
                    if (tx_triangle->intersect(ray, interval, mirror_record)) {
                        glm::vec3 tx_reflective_point = mirror_record.get_point();

                        ray = Ray{ rx_mirror_point, tx_reflective_point - rx_mirror_point };
                        interval = Interval{ 0.0f, glm::length(tx_reflective_point - rx_mirror_point) };
                        mirror_record.clear();
                        if (rx_triangle->intersect(ray, interval, mirror_record)) {
                            glm::vec3 rx_reflective_point = mirror_record.get_point();

                            ReflectionRecord ref_record{ 2, std::vector<glm::vec3>{tx_pos} };
                            if (trace_direct(tx_pos, tx_reflective_point, ref_record) && trace_direct(tx_reflective_point, rx_reflective_point, ref_record) && trace_direct(rx_reflective_point, rx_pos, ref_record)) {
                                ref_records.emplace_back(ref_record);
                                is_reflect = true;
                            }
                        }
                    }
                }
            }
            return is_reflect;
        }

        void init_draw() {
            std::transform(m_ref_records.begin(), m_ref_records.end(), std::back_inserter(m_lines), [](const signal_tracer::ReflectionRecord& ref_record) {
                return signal_tracer::Line{ ref_record };
                });
        }

        void draw_radio_stations(cy::GLSLProgram& shader_program, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
            shader_program.Bind();
            for (std::size_t i = 0; i < m_station_positions.size(); ++i) {
                // get color from StationColor enum class
                StationColor color = static_cast<StationColor>(i);
                shader_program.SetUniform("color", get_station_color(color));
                m_radio_object.draw(shader_program, glm::translate(model, m_station_positions[i]), view, projection);
            }
        }

        void draw(cy::GLSLProgram& shader_program, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
            shader_program.Bind();
            for (auto& line : m_lines) {
                if (line.get_reflection_count() == m_display_reflection_count) {
                    // shader_program.SetUniformVec3("color", glm::vec3{ 0.0f, 0.0f, 1.0f });
                    line.draw(shader_program, model, view, projection);
                }


            }
        }

        bool is_direct_lighting() const { return m_is_direct_lighting; }

        friend std::ostream& operator<<(std::ostream& os, const SignalTracer& signal_tracer) {
            os << "SignalTracer: " << std::endl;
            for (const auto& ref_record : signal_tracer.m_ref_records) {
                os << ref_record;
            }
            return os;
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

    private:
        // Tracing
        std::vector<std::shared_ptr<Triangle>> m_triangles{};
        std::shared_ptr<BVH> m_bvh{};
        std::vector<ReflectionRecord> m_ref_records{};
        int m_max_reflection{ 1 };
        bool m_is_direct_lighting{ false };
        std::vector<glm::vec3> m_station_positions{};

        // Drawing
        Cubesphere m_radio_object{ 0.25f, 3, true };
        std::vector<signal_tracer::Line> m_lines{};
        int m_display_reflection_count{ 0 };
    };
}



#endif // !SIGNAL_TRACER_HPP