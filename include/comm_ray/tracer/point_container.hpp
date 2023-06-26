// #pragma once

// #ifndef POINT_CONTAINER_HPP
// #define POINT_CONTAINER_HPP

// #include "glm/glm.hpp"
// #include <vector>
// #include <iostream>
// #include "glm/gtx/string_cast.hpp"

// namespace signal_tracer {
//     class PointContainer {
//     public:
//         PointContainer() = default;
//         PointContainer(const glm::vec3& point) : m_points{ std::vector<glm::vec3>{point} } {}
//         PointContainer(const std::vector<glm::vec3>& points) : m_points{ points } {}
//         PointContainer(const PointContainer& rhs) : m_points{ rhs.m_points } {}
//         PointContainer(PointContainer&& rhs) noexcept : m_points{ std::move(rhs.m_points) } {}
//         PointContainer& operator=(const PointContainer& rhs) {
//             if (this == &rhs) {
//                 return *this;
//             }
//             m_points = rhs.m_points;
//             return *this;
//         }
//         PointContainer& operator=(PointContainer&& rhs) noexcept {
//             if (this == &rhs) {
//                 return *this;
//             }
//             m_points = std::move(rhs.m_points);
//             return *this;
//         }
//         ~PointContainer() = default;

//         const std::vector<glm::vec3>& points() const { return m_points; }
//         void points(const std::vector<glm::vec3>& points) { m_points = points; }
//         void add(const glm::vec3& point) { m_points.push_back(point); }
//         void add(const std::vector<glm::vec3>& points) { m_points.insert(m_points.end(), points.begin(), points.end()); }
//         void clear() { m_points.clear(); }

//         std::vector<glm::vec3>::iterator begin() { return m_points.begin(); }
//         std::vector<glm::vec3>::iterator end() { return m_points.end(); }
//         std::vector<glm::vec3>::const_iterator begin() const { return m_points.begin(); }
//         std::vector<glm::vec3>::const_iterator end() const { return m_points.end(); }

//         std::vector<glm::vec3>::reverse_iterator rbegin() { return m_points.rbegin(); }
//         std::vector<glm::vec3>::reverse_iterator rend() { return m_points.rend(); }
//         std::vector<glm::vec3>::const_reverse_iterator rbegin() const { return m_points.rbegin(); }
//         std::vector<glm::vec3>::const_reverse_iterator rend() const { return m_points.rend(); }

//         std::vector<glm::vec3>::const_iterator cbegin() const { return m_points.cbegin(); }
//         std::vector<glm::vec3>::const_iterator cend() const { return m_points.cend(); }
//         std::vector<glm::vec3>::const_reverse_iterator crbegin() const { return m_points.crbegin(); }
//         std::vector<glm::vec3>::const_reverse_iterator crend() const { return m_points.crend(); }

//         glm::vec3& operator[](size_t index) { return m_points[index]; }
//         const glm::vec3& operator[](size_t index) const { return m_points[index]; }

//         size_t size() const { return m_points.size(); }
//         bool empty() const { return m_points.empty(); }

//         friend std::ostream& operator<<(std::ostream& os, const PointContainer& rhs) {
//             os << "PointContainer: " << std::endl;
//             for (const auto& point : rhs.m_points) {
//                 os << "\t" << glm::to_string(point) << std::endl;
//             }
//             return os;
//         }

//     private:
//         std::vector<glm::vec3> m_points{};
//     };
// }

// #endif // !POINT_CONTAINER_HPP