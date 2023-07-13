#pragma once

#ifndef TRIANGLE_DRAW_HPP
#define TRIANGLE_DRAW_HPP


#include "glad/gl.h"
#include "glm/glm.hpp"
#include "cyGL.h"
#include "glm/gtc/type_ptr.hpp"


namespace signal_tracer {
    class TriangleDrawing {
    public:
        TriangleDrawing() = default;
        TriangleDrawing(const std::vector<glm::vec3>& points) : m_points(points) {
            setup_draw();
        }

        void draw(cy::GLSLProgram& shader_program, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
            shader_program.Bind();
            glm::mat3 normal_matrix{ glm::mat3(glm::transpose(glm::inverse(view * model))) };
            shader_program.SetUniformMatrix4("model_view", glm::value_ptr(view * model), 1, false);
            shader_program.SetUniformMatrix4("model_view_projection", glm::value_ptr(projection * view * model), 1, false);
            shader_program.SetUniformMatrix3("normal_matrix", glm::value_ptr(normal_matrix), 1, false);
            glBindVertexArray(m_vao);
            glDrawArrays(GL_TRIANGLES, 0, m_points.size());
        };

    private:
        // Remember to activate the program before setting uniforms!
        void setup_draw() {
            glGenVertexArrays(1, &m_vao);
            glBindVertexArray(m_vao);

            glGenBuffers(1, &m_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferData(GL_ARRAY_BUFFER, m_points.size() * sizeof(glm::vec3), &m_points[0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));

            // unbind
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

    private:
        std::vector<glm::vec3> m_points;
        unsigned int m_vao{}, m_vbo{}, m_ebo{};
    };
}

#endif // !TRIANGLE_DRAW_HPP