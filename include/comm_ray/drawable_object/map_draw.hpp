#pragma once

#ifndef MAP_DRAW_HPP
#define MAP_DRAW_HPP

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "cyGL.h"
#include "containers.hpp"
#include "drawable.hpp"
#include <string>
#include <vector>

namespace SignalTracer {
    class MapDrawing : public Drawable {
    public:
        MapDrawing(const int& num_row, const int& num_col, const std::vector<Cell>& cells)
            : m_cells{ cells }
            , m_indices{ make_indices(num_row, num_col) } {

            for (int i = 0; i < num_row; ++i) {
                for (int j = 0; j < num_col; ++j) {
                    // cout strength
                    std::cout << m_cells[i * num_col + j].strength << "\t";
                }
                std::cout << "\n";
            }
            setup_draw();
        }

        // prepare indices
        std::vector<uint> make_indices(int num_row, int num_col) {
            std::vector<uint> indices((num_row - 1) * (num_col - 1) * 6, 0);

            for (int i = 0; i < num_row - 1; ++i) {
                for (int j = 0; j < num_col - 1; ++j) {
                    int current_row = i * num_col;
                    int next_row = (i + 1) * num_col;
                    int cur_idx = i * (num_col - 1) * 6 + j * 6;

                    // triangle 1
                    indices[cur_idx] = current_row + j;
                    indices[cur_idx + 1] = next_row + j;
                    indices[cur_idx + 2] = next_row + j + 1;

                    // triangle 2
                    indices[cur_idx + 3] = next_row + j + 1;
                    indices[cur_idx + 4] = current_row + j + 1;
                    indices[cur_idx + 5] = current_row + j;

                }
            }

            return indices;
        }

        std::vector<glm::vec3> make_strengths(const std::vector<float>& strengths) {
            std::vector<glm::vec3> strengths_vec(strengths.size(), glm::vec3(0));

            for (std::size_t i = 0; i < strengths.size(); ++i) {
                strengths_vec[i] = glm::vec3(strengths[i]);
            }

            return strengths_vec;
        }

        void draw(cy::GLSLProgram& shader_program, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const override {
            shader_program.Bind();
            glm::mat3 normal_matrix{ glm::mat3(glm::transpose(glm::inverse(view * model))) };
            shader_program.SetUniformMatrix4("model_view", glm::value_ptr(view * model), 1, false);
            shader_program.SetUniformMatrix4("model_view_projection", glm::value_ptr(projection * view * model), 1, false);
            shader_program.SetUniformMatrix3("normal_matrix", glm::value_ptr(normal_matrix), 1, false);

            glBindVertexArray(m_vao);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        };

    private:

        // Remember to activate the program before setting uniforms!
        void setup_draw() const override {
            glGenVertexArrays(1, &m_vao);
            glBindVertexArray(m_vao);

            glGenBuffers(1, &m_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferData(GL_ARRAY_BUFFER, m_cells.size() * sizeof(Cell), &m_cells[0], GL_STATIC_DRAW);

            glGenBuffers(1, &m_ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Cell), (void*) 0);

            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Cell), (void*) offsetof(Cell, strength));

            glBindVertexArray(0);
        }

    private:
        std::vector<Cell> m_cells{};
        std::vector<uint> m_indices{};
    };
}

#endif // !MAP_DRAW_HPP