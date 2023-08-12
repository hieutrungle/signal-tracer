// #pragma once

// #ifndef SIGNAL_TRACER_HPP
// #define SIGNAL_TRACER_HPP

// #include "base_tracer.hpp"
// #include "model.hpp"
// #include "bvh_map.hpp"
// #include "constant.hpp"
// #include "reflection_record.hpp"
// #include "triangle.hpp"
// #include "utils.hpp"
// #include "glm/glm.hpp"

// #include <functional>
// #include <iostream>
// #include <memory>
// #include <string>
// #include <vector>


// namespace SignalTracer {

//     /**
//     @brief
//     Instructions:

//     1 Initilization Part:
//     - Initialize a ImagingTracer object with a vector of Mesh objects.
//     - Call the tracing() function to get all reflection records.
//     - Call the init_draw() function to initialize the Line objects for drawing.

//     2 Drawing Part:
//     - Call the draw_radio_stations() function to draw the radio stations.
//     - Call the draw() function to draw the Line objects.
//     */
//     class ImagingTracer : public BaseTracer {
//     public:
//         ImagingTracer() = default;
//         ImagingTracer(const std::vector<Model>& models, int max_reflection = 2)
//             : BaseTracer{ models, max_reflection } {}
//         ImagingTracer(const std::vector<std::reference_wrapper<Model>>& models, int max_reflection = 2)
//             : BaseTracer{ models, max_reflection } {}

//         ~ImagingTracer() override = default;

//         // Copy constructor
//         ImagingTracer(const ImagingTracer& signal_tracer) : BaseTracer{ signal_tracer } {
//             m_is_direct_lighting = signal_tracer.m_is_direct_lighting;
//         }

//         // Move constructor
//         ImagingTracer(ImagingTracer&& signal_tracer) : BaseTracer{ std::move(signal_tracer) } {
//             m_is_direct_lighting = std::move(signal_tracer.m_is_direct_lighting);
//         }

//         // Assignment operator
//         ImagingTracer& operator=(const ImagingTracer& signal_tracer) {
//             BaseTracer::operator=(signal_tracer);
//             m_is_direct_lighting = signal_tracer.m_is_direct_lighting;
//             return *this;
//         }

//         // Move assignment operator
//         ImagingTracer& operator=(ImagingTracer&& signal_tracer) noexcept {
//             BaseTracer::operator=(std::move(signal_tracer));
//             m_is_direct_lighting = std::move(signal_tracer.m_is_direct_lighting);
//             return *this;
//         }

//         void reset() override {
//             m_is_direct_lighting = false;
//         }

//         void trace_rays(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<PathRecord>& ref_records) override {
//             reset();
//             std::clog << "tx position: " << glm::to_string(tx_pos) << std::endl;
//             std::clog << "rx position: " << glm::to_string(rx_pos) << std::endl;

//             PathRecord ref_record{ 0, std::vector<glm::vec3>{tx_pos}, std::vector<std::shared_ptr<Material>>{} };
//             if (m_max_reflection >= 0) {
//                 if (is_ray_direct(tx_pos, rx_pos, ref_record)) {
//                     ref_records.emplace_back(ref_record);
//                 }
//             }
//             if (m_max_reflection >= 1) {
//                 if (is_ray_reflected(tx_pos, rx_pos, ref_records)) {}
//             }
//             if (m_max_reflection >= 2) {
//                 if (is_ray_double_reflect(tx_pos, rx_pos, ref_records)) {}
//             }
//         }


//         /// @brief Check if there is a direct path from tx to rx. If so, add the point to the ref_points vector of ref_record
//         /// @param tx_pos 
//         /// @param rx_pos 
//         /// @param ref_record 
//         /// @return 
//         bool is_ray_direct(const glm::vec3& tx_pos, const glm::vec3& rx_pos, PathRecord& ref_record) const {
//             Ray ray(tx_pos, rx_pos - tx_pos);
//             Interval interval{ Constant::EPSILON, glm::length(rx_pos - tx_pos) - Constant::EPSILON };
//             IntersectRecord record{};
//             if (!m_bvh.is_hit(ray, interval, record)) {
//                 ref_record.add_point(rx_pos);
//                 return true;
//             }
//             return false;
//         }

//         /// @brief Add the reflection records to the ref_records vector if the ray is reflected
//         /// @param tx_pos 
//         /// @param rx_pos 
//         /// @param ref_records 
//         /// @return 
//         bool is_ray_reflected(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<PathRecord>& ref_records) const {
//             bool is_reflect = false;
//             for (const auto& triangle : m_triangles) {
//                 // cast to triangle
//                 std::shared_ptr<Triangle> triangle_ptr{ std::static_pointer_cast<Triangle>(triangle) };
//                 glm::vec3 mirror_point = triangle_ptr->get_mirror_point(tx_pos);
//                 Ray ray{ mirror_point, rx_pos - mirror_point };
//                 Interval interval{ Constant::EPSILON, glm::length(rx_pos - mirror_point) - Constant::EPSILON };
//                 IntersectRecord record{};
//                 if (triangle->is_hit(ray, interval, record)) {
//                     glm::vec3 reflective_point = record.get_point();

//                     PathRecord ref_record{ 1, std::vector<glm::vec3>{tx_pos}, std::vector<std::shared_ptr<Material>>{record.get_material_ptr()} };
//                     if (is_ray_direct(tx_pos, reflective_point, ref_record) && is_ray_direct(reflective_point, rx_pos, ref_record)) {
//                         ref_records.emplace_back(ref_record);
//                         is_reflect = true;
//                     }
//                 }
//             }
//             return is_reflect;
//         }

//         bool is_ray_double_reflect(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<PathRecord>& ref_records) const {
//             bool is_reflect = false;
//             // Setup mirror points of tx and rx
//             std::vector<glm::vec3> tx_mirror_points{};
//             std::vector<glm::vec3> rx_mirror_points{};
//             for (const auto& triangle : m_triangles) {
//                 std::shared_ptr<Triangle> triangle_ptr{ std::static_pointer_cast<Triangle>(triangle) };
//                 tx_mirror_points.emplace_back(triangle_ptr->get_mirror_point(tx_pos));
//                 rx_mirror_points.emplace_back(triangle_ptr->get_mirror_point(rx_pos));
//             }

//             // Find the first reflection point
//             for (std::size_t i = 0; i < m_triangles.size(); ++i) {

//                 glm::vec3 tx_mirror_point = tx_mirror_points[i];
//                 std::shared_ptr<Triangle> tx_triangle{ std::static_pointer_cast<Triangle>(m_triangles[i]) };

//                 for (std::size_t j = 0; j < m_triangles.size(); ++j) {
//                     if (i == j) continue;
//                     glm::vec3 rx_mirror_point = rx_mirror_points[j];
//                     std::shared_ptr<Triangle> rx_triangle{ std::static_pointer_cast<Triangle>(m_triangles[j]) };

//                     Ray ray{ tx_mirror_point, rx_mirror_point - tx_mirror_point };
//                     Interval interval{ 0.0f, glm::length(rx_mirror_point - tx_mirror_point) };
//                     IntersectRecord tx_mirror_record{};
//                     if (tx_triangle->is_hit(ray, interval, tx_mirror_record)) {
//                         glm::vec3 tx_reflective_point = tx_mirror_record.get_point();

//                         ray = Ray{ rx_mirror_point, tx_reflective_point - rx_mirror_point };
//                         interval = Interval{ 0.0f, glm::length(tx_reflective_point - rx_mirror_point) };
//                         IntersectRecord rx_mirror_record{};
//                         if (rx_triangle->is_hit(ray, interval, rx_mirror_record)) {
//                             glm::vec3 rx_reflective_point = rx_mirror_record.get_point();

//                             PathRecord ref_record{ 2, std::vector<glm::vec3>{tx_pos}, std::vector<std::shared_ptr<Material>>{ tx_mirror_record.get_material_ptr(), rx_mirror_record.get_material_ptr()} };
//                             if (is_ray_direct(tx_pos, tx_reflective_point, ref_record) && is_ray_direct(tx_reflective_point, rx_reflective_point, ref_record) && is_ray_direct(rx_reflective_point, rx_pos, ref_record)) {
//                                 ref_records.emplace_back(ref_record);
//                                 is_reflect = true;
//                             }
//                         }
//                     }
//                 }
//             }
//             return is_reflect;
//         }

//         bool is_direct_lighting() const { return m_is_direct_lighting; }

//         std::ostream& print(std::ostream& out) const override {
//             out << "Signal Tracing: \n ";
//             return out;
//         }

//     private:
//         bool m_is_direct_lighting{ false };
//     };
// }

// #endif // !SIGNAL_TRACER_HPP






// // #pragma once

// // #ifndef SIGNAL_TRACER_HPP
// // #define SIGNAL_TRACER_HPP

// // #include "tracer_interface.hpp"
// // #include "model.hpp"
// // #include "bvh_map.hpp"
// // #include "constant.hpp"
// // #include "reflection_record.hpp"
// // #include "triangle.hpp"
// // #include "utils.hpp"
// // #include "glm/glm.hpp"
// // #include <iostream>
// // #include <memory>
// // #include <string>
// // #include <vector>


// // namespace SignalTracer {

// //     /**
// //     @brief
// //     Instructions:

// //     1 Initilization Part:
// //     - Initialize a SignalTracer object with a vector of Mesh objects.
// //     - Call the tracing() function to get all reflection records.
// //     - Call the init_draw() function to initialize the Line objects for drawing.

// //     2 Drawing Part:
// //     - Call the draw_radio_stations() function to draw the radio stations.
// //     - Call the draw() function to draw the Line objects.
// //     */
// //     class SignalTracer : public TracerInterface {
// //     public:
// //         SignalTracer() = default;
// //         SignalTracer(const std::vector<Model>& models, int max_reflection = 2)
// //             : m_max_reflection{ max_reflection }
// //             , m_triangles{ init_triangles(models) }
// //             , m_bvh{ std::make_shared<BVHAccel>(m_triangles, 0, m_triangles.size()) } {}

// //         SignalTracer(const std::vector<std::shared_ptr<Model>>& model_ptrs, int max_reflection = 2)
// //             : m_max_reflection{ max_reflection }
// //             , m_triangles{ init_triangles(model_ptrs) }
// //             , m_bvh{ std::make_shared<BVHAccel>(m_triangles, 0, m_triangles.size()) } {}

// //         const std::shared_ptr<BVHAccel>& bvh() const { return m_bvh; }
// //         int max_reflection() const { return m_max_reflection; }

// //         virtual ~SignalTracer() = default;

// //         // Copy constructor
// //         SignalTracer(const SignalTracer& signal_tracer) {
// //             *this = signal_tracer;
// //         }

// //         // Move constructor
// //         SignalTracer(SignalTracer&& signal_tracer) noexcept {
// //             *this = std::move(signal_tracer);
// //         }

// //         // Assignment operator
// //         SignalTracer& operator=(const SignalTracer& signal_tracer) {
// //             m_triangles = signal_tracer.m_triangles;
// //             m_bvh = signal_tracer.m_bvh;
// //             m_max_reflection = signal_tracer.m_max_reflection;
// //             m_is_direct_lighting = signal_tracer.m_is_direct_lighting;
// //             return *this;
// //         }

// //         // Move assignment operator
// //         SignalTracer& operator=(SignalTracer&& signal_tracer) noexcept {
// //             m_triangles = std::move(signal_tracer.m_triangles);
// //             m_bvh = std::move(signal_tracer.m_bvh);
// //             m_max_reflection = std::move(signal_tracer.m_max_reflection);
// //             m_is_direct_lighting = std::move(signal_tracer.m_is_direct_lighting);
// //             return *this;
// //         }

// //         void reset() override {
// //             m_is_direct_lighting = false;
// //         }

// //         void trace_rays(glm::vec3& tx_pos, glm::vec3& rx_pos, std::vector<PathRecord>& ref_records) override {
// //             reset();
// //             std::clog << "tx position: " << glm::to_string(tx_pos) << std::endl;
// //             std::clog << "rx position: " << glm::to_string(rx_pos) << std::endl;

// //             PathRecord ref_record{ 0, std::vector<glm::vec3>{tx_pos}, std::vector<std::shared_ptr<Material>>{} };
// //             if (m_max_reflection >= 0) {
// //                 if (is_ray_direct(tx_pos, rx_pos, ref_record)) {
// //                     ref_records.emplace_back(ref_record);
// //                 }
// //             }
// //             if (m_max_reflection >= 1) {
// //                 if (is_ray_reflected(tx_pos, rx_pos, ref_records)) {}
// //             }
// //             if (m_max_reflection >= 2) {
// //                 if (is_ray_double_reflect(tx_pos, rx_pos, ref_records)) {}
// //             }
// //         }


// //         /// @brief Check if there is a direct path from tx to rx. If so, add the point to the ref_points vector of ref_record
// //         /// @param tx_pos 
// //         /// @param rx_pos 
// //         /// @param ref_record 
// //         /// @return 
// //         bool is_ray_direct(const glm::vec3& tx_pos, const glm::vec3& rx_pos, PathRecord& ref_record) const {
// //             Ray ray(tx_pos, rx_pos - tx_pos);
// //             Interval interval{ Constant::EPSILON, glm::length(rx_pos - tx_pos) - Constant::EPSILON };
// //             IntersectRecord record{};
// //             if (!m_bvh->is_hit(ray, interval, record)) {
// //                 ref_record.ref_points.emplace_back(rx_pos);
// //                 return true;
// //             }
// //             return false;
// //         }

// //         /// @brief Add the reflection records to the ref_records vector if the ray is reflected
// //         /// @param tx_pos 
// //         /// @param rx_pos 
// //         /// @param ref_records 
// //         /// @return 
// //         bool is_ray_reflected(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<PathRecord>& ref_records) const {
// //             bool is_reflect = false;
// //             for (const auto& triangle : m_triangles) {
// //                 glm::vec3 mirror_point = triangle->get_mirror_point(tx_pos);
// //                 Ray ray{ mirror_point, rx_pos - mirror_point };
// //                 Interval interval{ Constant::EPSILON, glm::length(rx_pos - mirror_point) - Constant::EPSILON };
// //                 IntersectRecord record{};
// //                 if (triangle->is_hit(ray, interval, record)) {
// //                     glm::vec3 reflective_point = record.get_point();

// //                     PathRecord ref_record{ 1, std::vector<glm::vec3>{tx_pos}, std::vector<std::shared_ptr<Material>>{record.get_material_ptr()} };
// //                     if (is_ray_direct(tx_pos, reflective_point, ref_record) && is_ray_direct(reflective_point, rx_pos, ref_record)) {
// //                         ref_records.emplace_back(ref_record);
// //                         is_reflect = true;
// //                     }
// //                 }
// //             }
// //             return is_reflect;
// //         }

// //         bool is_ray_double_reflect(const glm::vec3& tx_pos, const glm::vec3& rx_pos, std::vector<PathRecord>& ref_records) const {
// //             bool is_reflect = false;
// //             // Setup mirror points of tx and rx
// //             std::vector<glm::vec3> tx_mirror_points{};
// //             std::vector<glm::vec3> rx_mirror_points{};
// //             for (const auto& triangle : m_triangles) {
// //                 tx_mirror_points.emplace_back(triangle->get_mirror_point(tx_pos));
// //                 rx_mirror_points.emplace_back(triangle->get_mirror_point(rx_pos));
// //             }

// //             // Find the first reflection point
// //             for (std::size_t i = 0; i < m_triangles.size(); ++i) {
// //                 glm::vec3 tx_mirror_point = tx_mirror_points[i];
// //                 std::shared_ptr<Triangle> tx_triangle{ m_triangles[i] };

// //                 for (std::size_t j = 0; j < m_triangles.size(); ++j) {
// //                     if (i == j) continue;
// //                     glm::vec3 rx_mirror_point = rx_mirror_points[j];
// //                     std::shared_ptr<Triangle> rx_triangle{ m_triangles[j] };

// //                     Ray ray{ tx_mirror_point, rx_mirror_point - tx_mirror_point };
// //                     Interval interval{ 0.0f, glm::length(rx_mirror_point - tx_mirror_point) };
// //                     IntersectRecord tx_mirror_record{};
// //                     if (tx_triangle->is_hit(ray, interval, tx_mirror_record)) {
// //                         glm::vec3 tx_reflective_point = tx_mirror_record.get_point();

// //                         ray = Ray{ rx_mirror_point, tx_reflective_point - rx_mirror_point };
// //                         interval = Interval{ 0.0f, glm::length(tx_reflective_point - rx_mirror_point) };
// //                         IntersectRecord rx_mirror_record{};
// //                         if (rx_triangle->is_hit(ray, interval, rx_mirror_record)) {
// //                             glm::vec3 rx_reflective_point = rx_mirror_record.get_point();

// //                             PathRecord ref_record{ 2, std::vector<glm::vec3>{tx_pos}, std::vector<std::shared_ptr<Material>>{ tx_mirror_record.get_material_ptr(), rx_mirror_record.get_material_ptr()} };
// //                             if (is_ray_direct(tx_pos, tx_reflective_point, ref_record) && is_ray_direct(tx_reflective_point, rx_reflective_point, ref_record) && is_ray_direct(rx_reflective_point, rx_pos, ref_record)) {
// //                                 ref_records.emplace_back(ref_record);
// //                                 is_reflect = true;
// //                             }
// //                         }
// //                     }
// //                 }
// //             }
// //             return is_reflect;
// //         }

// //         bool is_direct_lighting() const { return m_is_direct_lighting; }

// //         float calc_total_receiving_power(const std::vector<PathRecord>& ref_records) const {
// //             float total_power{ 0.0f };
// //             // using accumulate
// //             total_power = std::accumulate(ref_records.begin(), ref_records.end(), 0.0f, [](float sum, const PathRecord& record) {
// //                 return sum + Utils::dB_to_linear(record.signal_strength);
// //                 });
// //             return Utils::linear_to_dB(total_power);
// //         }

// //         std::ostream& print(std::ostream& out) const override {
// //             out << "Signal Tracing: \n ";
// //             return out;
// //         }

// //     protected:

// //         std::vector<std::shared_ptr<Triangle>> init_triangles(const std::vector<Model>& models) {
// //             std::vector<std::shared_ptr<Triangle>> triangles{};
// //             std::vector<Vertex> vertex_buffer;
// //             vertex_buffer.reserve(3);
// //             int triangle_count{ 0 };
// //             int vertex_count{ 0 };
// //             for (const auto& model : models) {
// //                 for (const auto& mesh : model.get_meshes()) {
// //                     for (const auto& idx : mesh.get_indices()) {
// //                         vertex_count++;
// //                         vertex_buffer.push_back(mesh.get_vertices()[idx]);
// //                         triangle_count++;
// //                         if (triangle_count == 3) {
// //                             triangles.emplace_back(std::make_shared<Triangle>(
// //                                 vertex_buffer[0].position,
// //                                 vertex_buffer[1].position,
// //                                 vertex_buffer[2].position)
// //                             );
// //                             vertex_buffer.clear();
// //                             triangle_count = 0;
// //                         }
// //                     }
// //                 }
// //             }
// //             std::cout << "Triangle count: " << triangles.size() << std::endl;
// //             std::cout << "Vertex count: " << vertex_count << std::endl;
// //             return triangles;
// //         }

// //         std::vector<std::shared_ptr<Triangle>> init_triangles(const std::vector<std::shared_ptr<Model>>& model_ptrs) {
// //             std::vector<std::shared_ptr<Triangle>> triangles{};
// //             std::vector<Vertex> vertex_buffer;
// //             vertex_buffer.reserve(3);
// //             int triangle_count{ 0 };
// //             int vertex_count{ 0 };
// //             for (const auto& model_ptr : model_ptrs) {
// //                 for (const auto& mesh : model_ptr->get_meshes()) {
// //                     for (const auto& idx : mesh.get_indices()) {
// //                         vertex_count++;
// //                         vertex_buffer.push_back(mesh.get_vertices()[idx]);
// //                         triangle_count++;
// //                         if (triangle_count == 3) {
// //                             triangles.emplace_back(std::make_shared<Triangle>(
// //                                 vertex_buffer[0].position,
// //                                 vertex_buffer[1].position,
// //                                 vertex_buffer[2].position)
// //                             );
// //                             vertex_buffer.clear();
// //                             triangle_count = 0;
// //                         }
// //                     }
// //                 }
// //             }
// //             std::cout << "Triangle count: " << triangles.size() << std::endl;
// //             std::cout << "Vertex count: " << vertex_count << std::endl;
// //             return triangles;
// //         }

// //     private:
// //         int m_max_reflection{ 2 };
// //         std::vector<std::shared_ptr<Triangle>> m_triangles{};
// //         std::shared_ptr<BVHAccel> m_bvh{};
// //         bool m_is_direct_lighting{ false };
// //     };
// // }

// // #endif // !SIGNAL_TRACER_HPP