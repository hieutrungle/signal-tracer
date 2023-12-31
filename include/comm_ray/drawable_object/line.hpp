#pragma once

#ifndef LINE_HPP
#define LINE_HPP

#include "glad/gl.h"
#include "cyGL.h"
#include "drawable.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"
#include <iostream>

namespace SignalTracer {
    class Line : public Drawable {
    public:
        Line() = default;
        Line(const std::vector<glm::vec3>& ref_points, const int& reflection_count)
            : Drawable{}
            , m_points{ init_points(ref_points) }
            , m_reflection_count{ reflection_count } {
            setup_draw();
        }

        // copy constructor
        Line(const Line& other)
            : Drawable{}
            , m_points{ other.m_points }
            , m_reflection_count{ other.m_reflection_count } {
            setup_draw();
        }

        // move constructor
        Line(Line&& other) noexcept
            : Drawable{}
            , m_points{ std::move(other.m_points) }
            , m_reflection_count{ other.m_reflection_count } {
            setup_draw();
        }

        // copy assignment
        Line& operator=(const Line& other) {
            if (this != &other) {
                m_points = other.m_points;
                m_reflection_count = other.m_reflection_count;
                setup_draw();
            }
            return *this;
        }

        // move assignment
        Line& operator=(Line&& other) noexcept {
            if (this != &other) {
                m_points = std::move(other.m_points);
                m_reflection_count = other.m_reflection_count;
                setup_draw();
            }
            return *this;
        }

        ~Line() {}

        std::vector<glm::vec3> init_points(const std::vector<glm::vec3>& ref_points) const {
            std::vector<glm::vec3> points{};
            for (std::size_t i = 0; i < ref_points.size(); ++i) {
                if (i != 0 && i != ref_points.size() - 1) {
                    points.push_back(ref_points[i]);
                    points.push_back(ref_points[i]);
                }
                else {
                    points.push_back(ref_points[i]);
                }
            }
            return points;
        }

        void update_points(const std::vector<glm::vec3>& ref_points) {
            m_points = init_points(ref_points);
            setup_draw();
        }

        int get_reflection_count() const {
            return m_reflection_count;
        }

        void draw(cy::GLSLProgram& shader_program, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const override {
            shader_program.Bind();
            glm::mat3 normal_matrix{ glm::mat3(glm::transpose(glm::inverse(view * model))) };
            shader_program.SetUniformMatrix4("model_view", glm::value_ptr(view * model), 1, false);
            shader_program.SetUniformMatrix4("model_view_projection", glm::value_ptr(projection * view * model), 1, false);
            shader_program.SetUniformMatrix3("normal_matrix", glm::value_ptr(normal_matrix), 1, false);
            glBindVertexArray(m_vao);
            glDrawArrays(GL_LINES, 0, m_points.size());
        };

        std::ostream& print(std::ostream& out) const override {
            out << "Line: " << std::endl;
            out << "Reflection Count: " << m_reflection_count << std::endl;
            out << "Points: ";
            for (auto& point : m_points) {
                out << glm::to_string(point) << std::endl;
            }
            return out;
        }

    protected:
        // Remember to activate the program before setting uniforms!
        void setup_draw() const override {
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
    };
}



#endif // !LINE_HPP