#pragma once

#ifndef COVERAGE_MAP_HPP
#define COVERAGE_MAP_HPP

#include "glm/glm.hpp"
#include "quad.hpp"
#include "containers.hpp"
#include <vector>

namespace SignalTracer {

    struct QuadIntersectRecord {
        std::vector<glm::vec3> points{};
        std::vector<float> strengths{};
    };

    /// @brief This is a container that stores the coverage map of a transmitter.
    /// @details The coverage map is a 2D map that stores the signal strength of a transmitter at each cell.
    /// The cell is defined by the center of the cell and the size of the cell.
    /// The signal strength is defined by the signal strength at the center of the cell.
    /// Interpolation between cell centers is used to calculate the signal strength at other points.
    class CoverageMap {
    public:
        CoverageMap(const Quad& quad, const float cell_size = 2.0f)
            : m_cm{ quad }
            , m_cell_size{ cell_size }
            , m_num_row{ int(m_cm.get_height() / cell_size) + 2 }
            , m_num_col{ int(m_cm.get_width() / cell_size) + 2 }
            , m_cells(m_num_row* m_num_col, Cell()) {
            // , m_points(m_num_row* m_num_col, glm::vec3(0))
            // // , m_indices((m_num_row - 1)* (m_num_col - 1) * 2, glm::uvec3(0))
            // , m_strengths(m_num_row* m_num_col, 0.0f) {

            glm::vec3 corner_point{ m_cm.get_corner_point() };
            glm::vec3 u_vec{ m_cm.get_unit_u() }; // normalized
            glm::vec3 v_vec{ m_cm.get_unit_v() }; // normalized

            // calculate the cell center points
            for (int i = 0; i < m_num_row; ++i) {
                for (int j = 0; j < m_num_col; ++j) {
                    m_cells[i * m_num_col + j].point = corner_point + i * cell_size * u_vec + j * cell_size * v_vec;
                }
            }

            // // std::cout
            // for (int i = 0; i < m_num_row; ++i) {
            //     for (int j = 0; j < m_num_col; ++j) {
            //         std::cout << m_cells[i * m_num_col + j].point.x << " " << m_cells[i * m_num_col + j].point.y << " " << m_cells[i * m_num_col + j].point.z << "\t";
            //     }
            //     std::cout << std::endl;
            // }

            // for (int i = 0; i < m_num_row; ++i) {
            //     for (int j = 0; j < m_num_col; ++j) {
            //         std::cout << i * m_num_col + j << "\t";
            //     }
            //     std::cout << std::endl;
            // }

            std::cout << "Coverage map is created." << std::endl;
        }

        CoverageMap(const Quad& quad, const std::vector<float>& strengths, const float cell_size = 2.0f)
            : CoverageMap(quad, cell_size) {
            for (int i = 0; i < m_num_row; ++i) {
                for (int j = 0; j < m_num_col; ++j) {
                    m_cells[i * m_num_col + j].strength = strengths[i * m_num_col + j];
                }
            }
        }

        int get_num_row() const {
            return m_num_row;
        }
        int get_num_col() const {
            return m_num_col;
        }
        std::vector<Cell> get_cells() const {
            return m_cells;
        }

        void set_strength(const glm::vec3 point, float strength) {
            int cell_index = find_cell_index(point);
            m_cells[cell_index].strength = strength;
        }

        /// @brief Get the signal strength at a point.
        /// @param point The point to get the signal strength.
        /// @return The signal strength at the point.
        float get_strength(const glm::vec3 point) {
            int cell_index = find_cell_index(point);
            return m_cells[cell_index].strength;
        }

        void add_strength(const glm::vec3 point, float strength) {
            int cell_index = find_cell_index(point);
            m_cells[cell_index].strength += strength;
        }

        int find_cell_index(const glm::vec3& point) {
            glm::vec3 u_vec = m_cm.get_unit_u();
            glm::vec3 v_vec = m_cm.get_unit_v();

            // project point into vector u and v
            glm::vec3 to_point_vec = point - m_cm.get_corner_point();
            glm::vec3 projection_u = glm::dot(to_point_vec, u_vec) * u_vec;
            glm::vec3 projection_v = glm::dot(to_point_vec, v_vec) * v_vec;

            // calculate the cell index
            int row = glm::vec3(projection_u).length() / m_cell_size;
            int col = glm::vec3(projection_v).length() / m_cell_size;
            int cell_index = row * m_num_col + col;

            std::cout << "point: " << point.x << " " << point.y << " " << point.z << std::endl;
            std::cout << "cell index: " << cell_index << std::endl;
            return cell_index;
        }

    private:
        Quad m_cm;
        float m_cell_size{ 2.0f };
        int m_num_row{};
        int m_num_col{};
        std::vector<Cell> m_cells{};
        // std::vector<glm::vec3> m_points{};
        // std::vector<float> m_strengths{};
    };
}

#endif // !COVERAGE_MAP_HPP