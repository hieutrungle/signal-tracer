#pragma once

#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "cyGL.h"
#include <iostream>

namespace SignalTracer {
    class Drawable {
    public:
        Drawable() = default;
        Drawable(const Drawable& other) = default;
        Drawable(Drawable&& other) noexcept = default;
        Drawable& operator=(const Drawable& other) = default;
        Drawable& operator=(Drawable&& other) noexcept = default;
        virtual ~Drawable() {
            glDeleteVertexArrays(1, &m_vao);
            glDeleteBuffers(1, &m_vbo);
            glDeleteBuffers(1, &m_ebo);
        };

        virtual void set_color(const glm::vec3& color) {
            m_color = color;
        }

        virtual void set_model_mat(const glm::mat4& model_mat) {
            m_model_mat = model_mat;
        }

        virtual glm::vec3 get_color() const {
            return m_color;
        }

        virtual glm::mat4 get_model_mat() const {
            return m_model_mat;
        }


        virtual std::ostream& print(std::ostream& out) const {
            return out;
        }

        friend std::ostream& operator<<(std::ostream& out, const Drawable& drawable) {
            return drawable.print(out);
        }

        virtual void draw(cy::GLSLProgram& shader_program, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const = 0;

    protected:
        virtual void setup_draw() const = 0;

    protected:
        glm::vec3 m_color{ 1.0f };
        glm::mat4 m_model_mat{ 1.0f };
        mutable unsigned int m_vao{};
        mutable unsigned int m_vbo{};
        mutable unsigned int m_ebo{};

    };
}

#endif // !DRAWABLE_HPP