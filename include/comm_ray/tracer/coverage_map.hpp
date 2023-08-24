#pragma once

#ifndef COVERAGE_MAP_HPP
#define COVERAGE_MAP_HPP

#include "glm/glm.hpp"
// #include "glm/gtx/string_cast.hpp"
#include "quad.hpp"
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
            , m_num_row{ int(m_cm.get_height() / cell_size) + 1 }
            , m_num_col{ int(m_cm.get_width() / cell_size) + 1 }
            , m_points(m_num_row* m_num_col, glm::vec3(0))
            // , m_indices((m_num_row - 1)* (m_num_col - 1) * 2, glm::uvec3(0))
            , m_strengths(m_num_row* m_num_col, 0.0f) {

            glm::vec3 corner_point{ m_cm.get_corner_point() };
            glm::vec3 u_vec{ m_cm.get_unit_u() }; // normalized
            glm::vec3 v_vec{ m_cm.get_unit_v() }; // normalized

            // calculate the cell center points
            for (int i = 0; i < m_num_row; ++i) {
                for (int j = 0; j < m_num_col; ++j) {
                    m_points[i * m_num_col + j] = corner_point + i * cell_size * u_vec + j * cell_size * v_vec;
                }
            }

            // std::cout
            for (int i = 0; i < m_num_row; ++i) {
                for (int j = 0; j < m_num_col; ++j) {
                    std::cout << m_points[i * m_num_col + j].x << " " << m_points[i * m_num_col + j].y << " " << m_points[i * m_num_col + j].z << "\t";
                }
                std::cout << std::endl;
            }

            for (int i = 0; i < m_num_row; ++i) {
                for (int j = 0; j < m_num_col; ++j) {
                    std::cout << i * m_num_col + j << "\t";
                }
                std::cout << std::endl;
            }

            std::cout << "Coverage map is created." << std::endl;
        }

        CoverageMap(const Quad& quad, const std::vector<float>& strengths, const float cell_size = 2.0f)
            : CoverageMap(quad, cell_size) {
            m_strengths = strengths;
        }

        void set_strengths(const std::vector<float>& strengths) {
            m_strengths = strengths;
        }

        void set_strength(const glm::vec3 pos, float strength) {
            // find the cell that contains the point
            int row = int((pos.x - m_cm.get_corner_point().x) / m_cell_size);
            int col = int((pos.z - m_cm.get_corner_point().z) / m_cell_size);
            int cell_index = row * m_num_col + col;

            // set the signal strength at the point
            m_strengths[cell_index] = strength;
        }

        int get_num_row() const {
            return m_num_row;
        }
        int get_num_col() const {
            return m_num_col;
        }
        std::vector<glm::vec3> get_points() const {
            return m_points;
        }
        std::vector<float> get_strengths() const {
            return m_strengths;
        }

        // TODO: need to test this function
        /// @brief Get the signal strength at a point.
        /// @param point The point to get the signal strength.
        /// @return The signal strength at the point.
        float get_strength(const glm::vec3 point) {
            // find the cell that contains the point
            int row = int((point.x - m_cm.get_corner_point().x) / m_cell_size);
            int col = int((point.z - m_cm.get_corner_point().z) / m_cell_size);
            int cell_index = row * m_num_col + col;

            // calculate the signal strength at the point
            return m_strengths[cell_index];
        }

    private:
        Quad m_cm;
        float m_cell_size{ 2.0f };
        int m_num_row{};
        int m_num_col{};
        std::vector<glm::vec3> m_points{};
        std::vector<float> m_strengths{};
    };
}

#endif // !COVERAGE_MAP_HPP