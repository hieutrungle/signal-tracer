#pragma once
#ifndef LIGHTING_HPP
#define LIGHTING_HPP

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "utils.hpp"
#include "cyGL.h"
#include <string>

namespace Lighting {

    class Light {
    public:
        Light(
            const glm::vec3& color,
            const float ambient,
            const float diffuse,
            const float specular,
            const std::string& name = "light")
            : m_color(color)
            , m_ambient(ambient)
            , m_diffuse(diffuse)
            , m_specular(specular)
            , m_name(name) {}

        virtual void init(cy::GLSLProgram& shader_program, const glm::mat4& UTILS_UNUSED_PARAM(view_matrix)) {
            shader_program.Bind();
            shader_program.SetUniform((m_name + ".ambient").c_str(), m_ambient * m_color);
            shader_program.SetUniform((m_name + ".diffuse").c_str(), m_diffuse * m_color);
            shader_program.SetUniform((m_name + ".specular").c_str(), m_specular * m_color);
        };

        const glm::vec3 color() const { return m_color; }
        float ambient() const { return m_ambient; }
        float diffuse() const { return m_diffuse; }
        float specular() const { return m_specular; }
        const std::string name() const { return m_name; }

        void color(const glm::vec3& color) { m_color = color; }
        void ambient(const float ambient) { m_ambient = ambient; }
        void diffuse(const float diffuse) { m_diffuse = diffuse; }
        void specular(const float specular) { m_specular = specular; }
        void name(const std::string& name) { m_name = name; }

    protected:
        glm::vec3 m_color{};
        float m_ambient{};
        float m_diffuse{};
        float m_specular{};
        std::string m_name{};
    };

    class DirectionalLight : public Light {
    public:
        DirectionalLight(
            const glm::vec3& direction,
            const glm::vec3& color,
            const float ambient,
            const float diffuse,
            const float specular,
            const std::string& name = "directional_light")
            : Light(color, ambient, diffuse, specular, name)
            , m_direction(direction) {}

        glm::vec3 direction() const { return m_direction; }
        void direction(const glm::vec3& direction) { m_direction = direction; }

        void init(cy::GLSLProgram& shader_program, const glm::mat4& view_matrix) override {
            Light::init(shader_program, view_matrix);
            glm::mat4 normal_matrix = glm::transpose(glm::inverse(view_matrix));
            glm::vec3 direction_camera = glm::vec3(normal_matrix * glm::vec4(m_direction, 0.0f));
            shader_program.SetUniform((m_name + ".direction").c_str(), direction_camera);
        }

    protected:
        glm::vec3 m_direction{};
    };

    class PointLight : public Light {
    public:
        PointLight(
            const glm::vec3& position,
            const glm::vec3& color,
            const float ambient,
            const float diffuse,
            const float specular,
            float constant = 1.0f,
            float linear = 0.09f,
            float quadratic = 0.032f,
            const std::string& name = "point_light")
            : Light(color, ambient, diffuse, specular, name)
            , m_position(position)
            , m_constant(constant)
            , m_linear(linear)
            , m_quadratic(quadratic) {}

        void init(cy::GLSLProgram& shader_program, const glm::mat4& view_matrix) override {
            Light::init(shader_program, view_matrix);
            glm::vec3 position_camera = glm::vec3(view_matrix * glm::vec4(m_position, 0.0f));
            shader_program.SetUniform((m_name + ".position").c_str(), position_camera);
            shader_program.SetUniform((m_name + ".constant").c_str(), m_constant);
            shader_program.SetUniform((m_name + ".linear").c_str(), m_linear);
            shader_program.SetUniform((m_name + ".quadratic").c_str(), m_quadratic);
        }

        glm::vec3 position() const { return m_position; }
        float constant() const { return m_constant; }
        float linear() const { return m_linear; }
        float quadratic() const { return m_quadratic; }

    protected:
        glm::vec3 m_position{};
        float m_constant{};
        float m_linear{};
        float m_quadratic{};
    };

    class SpotLight : public PointLight {
    public:
        SpotLight(
            const glm::vec3& position,
            const glm::vec3& direction,
            const glm::vec3& color,
            const float ambient,
            const float diffuse,
            const float specular,
            float constant = 1.0f,
            float linear = 0.09f,
            float quadratic = 0.032f,
            float inner_cut_off = glm::cos(glm::radians(12.5f)),
            float outer_cut_off = glm::cos(glm::radians(15.0f)),
            const std::string& name = "spot_light")
            : PointLight(position, color, ambient, diffuse, specular, constant, linear, quadratic, name)
            , m_direction(direction)
            , m_inner_cut_off(inner_cut_off)
            , m_outer_cut_off(outer_cut_off) {}

        void init(cy::GLSLProgram& shader_program, const glm::mat4& view_matrix) override {
            PointLight::init(shader_program, view_matrix);
            glm::mat4 view_inverse_transpose = glm::transpose(glm::inverse(view_matrix));
            glm::vec3 direction_camera = glm::vec3(view_inverse_transpose * glm::vec4(m_direction, 0.0f));
            shader_program.SetUniform((m_name + ".direction").c_str(), direction_camera);
            shader_program.SetUniform((m_name + ".inner_cut_off").c_str(), m_inner_cut_off);
            shader_program.SetUniform((m_name + ".outer_cut_off").c_str(), m_outer_cut_off);
        }

        glm::vec3 direction() const { return m_direction; }
        float cut_off() const { return m_inner_cut_off; }
        float outer_cut_off() const { return m_outer_cut_off; }

    protected:
        glm::vec3 m_direction{};
        float m_inner_cut_off{};
        float m_outer_cut_off{};
    };

    class FlashLight : public SpotLight {
    public:
        FlashLight(
            const glm::vec3& position,
            const glm::vec3& direction,
            const glm::vec3& color,
            const float ambient,
            const float diffuse,
            const float specular,
            float constant = 1.0f,
            float linear = 0.09f,
            float quadratic = 0.032f,
            float inner_cut_off = glm::cos(glm::radians(12.5f)),
            float outer_cut_off = glm::cos(glm::radians(15.0f)),
            const std::string& name = "flash_light")
            : SpotLight(position, direction, color, ambient, diffuse, specular, constant, linear, quadratic, inner_cut_off, outer_cut_off, name) {}

        void init(cy::GLSLProgram& shader_program, const glm::mat4& view_matrix) override {
            SpotLight::init(shader_program, view_matrix);
        }

        glm::vec3 direction() const { return m_direction; }
        float cut_off() const { return m_inner_cut_off; }
        float outer_cut_off() const { return m_outer_cut_off; }
    };

}; // namespace Light



#endif // !LIGHT_HPP