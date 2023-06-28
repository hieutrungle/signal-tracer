#pragma once

#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "cyGL.h"

namespace signal_tracer {
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

        virtual void draw(cy::GLSLProgram& shader_program, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const = 0;

    protected:
        virtual void setup_draw() const = 0;

    protected:
        mutable unsigned int m_vao{};
        mutable unsigned int m_vbo{};
        mutable unsigned int m_ebo{};

    };
}

#endif // !DRAWABLE_HPP