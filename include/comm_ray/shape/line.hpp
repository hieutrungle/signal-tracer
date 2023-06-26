#pragma once

#ifndef LINE_HPP
#define LINE_HPP

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "cyGL.h"
#include "glm/gtc/type_ptr.hpp"
#include "point_container.hpp"


namespace signal_tracer {
    class Line {

    public:
        Line() = default;
        Line(const ReflectionRecord& ref_record)
            : m_points{ init_points(ref_record) }
            , m_reflection_count{ ref_record.reflection_count } {
            setup_draw();
        }

        std::vector<glm::vec3> init_points(ReflectionRecord ref_record) const {
            std::vector<glm::vec3> points{};
            for (std::size_t i = 0; i < ref_record.ref_points.size(); ++i) {
                if (i != 0 && i != ref_record.ref_points.size() - 1) {
                    points.push_back(ref_record.ref_points[i]);
                    points.push_back(ref_record.ref_points[i]);
                }
                else {
                    points.push_back(ref_record.ref_points[i]);
                }
            }
            return points;
        }

        void update_points(const ReflectionRecord& ref_record) {
            m_points = init_points(ref_record);
            setup_draw();
        }

        int get_reflection_count() const {
            return m_reflection_count;
        }

        void draw(cy::GLSLProgram& shader_program, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
            shader_program.Bind();
            glm::mat4 normal_matrix = glm::transpose(glm::inverse(view * model));
            shader_program.SetUniformMatrix4("view", glm::value_ptr(view), 1, false);
            shader_program.SetUniformMatrix4("projection", glm::value_ptr(projection), 1, false);
            shader_program.SetUniformMatrix4("model", glm::value_ptr(model), 1, false);
            shader_program.SetUniformMatrix4("view_model", glm::value_ptr(view * model), 1, false);
            shader_program.SetUniformMatrix4("mvp", glm::value_ptr(projection * view * model), 1, false);
            shader_program.SetUniformMatrix4("normal_matrix", glm::value_ptr(normal_matrix), 1, false);
            glBindVertexArray(m_vao);
            glDrawArrays(GL_LINES, 0, m_points.size());
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
        std::vector<glm::vec3> m_points{};
        int m_reflection_count{};
        unsigned int m_vao{}, m_vbo{}, m_ebo{};
    };
}



#endif // !LINE_HPP