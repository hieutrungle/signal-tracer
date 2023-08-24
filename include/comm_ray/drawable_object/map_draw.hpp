#pragma once

#ifndef MAP_DRAW_HPP
#define MAP_DRAW_HPP

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "cyGL.h"
#include "drawable.hpp"
#include <string>
#include <vector>

namespace SignalTracer {
    class MapDrawing : public Drawable {
    public:
        // MapDrawing() = default;
        // MapDrawing(const std::vector<glm::vec3>& points) : m_points(points) {
        //     setup_draw();
        // }

        MapDrawing(const int& num_row, const int& num_col, const std::vector<glm::vec3>& vertices, const std::vector<float>& strengths)
            : m_vertices{ vertices }
            , m_indices{ make_indices(num_row, num_col) }
            , m_strengths{ strengths } {
            m_indices.reserve((num_row - 1) * (num_col - 1) * 2);

            setup_draw();
        }

        // copy constructor
        MapDrawing(const MapDrawing& other)
            : m_vertices{ other.m_vertices }
            , m_indices{ other.m_indices }
            , m_strengths{ other.m_strengths } {
            setup_draw();
        }

        // move constructor
        MapDrawing(MapDrawing&& other) noexcept
            : m_vertices{ std::move(other.m_vertices) }
            , m_indices{ std::move(other.m_indices) }
            , m_strengths{ std::move(other.m_strengths) } {
            setup_draw();
        }

        // copy assignment
        MapDrawing& operator=(const MapDrawing& other) {
            m_vertices = other.m_vertices;
            m_indices = other.m_indices;
            m_strengths = other.m_strengths;
            setup_draw();
            return *this;
        }

        // move assignment
        MapDrawing& operator=(MapDrawing&& other) noexcept {
            m_vertices = std::move(other.m_vertices);
            m_indices = std::move(other.m_indices);
            m_strengths = std::move(other.m_strengths);
            setup_draw();
            return *this;
        }

        // Destructor
        ~MapDrawing() override {
            Drawable::~Drawable();
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

            // // cout
            // for (std::size_t i = 0; i < indices.size(); i += 6) {
            //     std::cout << indices[i] << " " << indices[i + 1] << " " << indices[i + 2] << "\t" << indices[i + 3] << " " << indices[i + 4] << " " << indices[i + 5] << std::endl;
            // }

            return indices;
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
            glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);

            glGenBuffers(1, &m_ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);

            glBindVertexArray(0);
        }

    private:
        std::vector<glm::vec3> m_vertices{};
        std::vector<uint> m_indices{};
        std::vector<float> m_strengths{};
    };
}

#endif // !MAP_DRAW_HPP