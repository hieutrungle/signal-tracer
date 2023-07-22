#pragma once
#ifndef MESH_HPP
#define MESH_HPP

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "cyGL.h"
#include "drawable.hpp"
#include <string>
#include <vector>

namespace SignalTracer {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex_coords;

        // Overload cout
        friend std::ostream& operator<<(std::ostream& os, const Vertex& vertex) {
            os << "Vertex: " << '\n';
            os << "Position: " << vertex.position.x << ' ' << vertex.position.y << ' ' << vertex.position.z << '\n';
            os << "Normal: " << vertex.normal.x << ' ' << vertex.normal.y << ' ' << vertex.normal.z << '\n';
            os << "Tex coords: " << vertex.tex_coords.x << ' ' << vertex.tex_coords.y << '\n';
            return os;
        }
    };

    struct Texture {
        unsigned int id;
        std::string type;
        std::string path;

        // Overload cout
        friend std::ostream& operator<<(std::ostream& os, const Texture& texture) {
            os << "Texture: " << texture.path << '\n';
            os << "ID: " << texture.id << '\n';
            os << "Type: " << texture.type << '\n';
            return os;
        }
    };

    class Mesh : public Drawable {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
            : m_vertices(vertices)
            , m_indices(indices)
            , m_textures(textures) {
            setup_draw();
        }

        // copy constructor
        Mesh(const Mesh& mesh)
            : m_vertices(mesh.m_vertices)
            , m_indices(mesh.m_indices)
            , m_textures(mesh.m_textures) {
            setup_draw();
        }

        // move constructor
        Mesh(Mesh&& mesh) noexcept
            : m_vertices(std::move(mesh.m_vertices))
            , m_indices(std::move(mesh.m_indices))
            , m_textures(std::move(mesh.m_textures)) {
            setup_draw();
        }

        // copy assignment
        Mesh& operator=(const Mesh& mesh) {
            m_vertices = mesh.m_vertices;
            m_indices = mesh.m_indices;
            m_textures = mesh.m_textures;
            setup_draw();
            return *this;
        }

        // move assignment
        Mesh& operator=(Mesh&& mesh) noexcept {
            m_vertices = std::move(mesh.m_vertices);
            m_indices = std::move(mesh.m_indices);
            m_textures = std::move(mesh.m_textures);
            setup_draw();
            return *this;
        }

        void draw(cy::GLSLProgram& shader_program, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const override;

        const std::vector<Vertex>& get_vertices() const { return m_vertices; }
        const std::vector<unsigned int>& get_indices() const { return m_indices; }
        const std::vector<Texture>& get_textures() const { return m_textures; }

        void transform(const glm::mat4& model_mat);

        void setup_draw() const;

    private:
        std::vector<Vertex> m_vertices{};
        std::vector<unsigned int> m_indices{};
        std::vector<Texture> m_textures{};
        // unsigned int m_vao{}, m_vbo{}, m_ebo{};
    };


    /*
        ----------------------------------
        Implementation
        ----------------------------------
    */

    void Mesh::transform(const glm::mat4& model_mat) {
        glm::mat4 normal_matrix = glm::transpose(glm::inverse(model_mat));
        for (auto& vertex : m_vertices) {
            vertex.position = glm::vec3{ model_mat * glm::vec4{ vertex.position, 1.0f } };
            vertex.normal = glm::vec3{ normal_matrix * glm::vec4{ vertex.normal, 1.0f } };
        }
        setup_draw();
    }

    void Mesh::setup_draw() const {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, tex_coords));

        glBindVertexArray(0);
    }

    void Mesh::draw(cy::GLSLProgram& shader_program, const glm::mat4& UTILS_UNUSED_PARAM(model), const glm::mat4& UTILS_UNUSED_PARAM(view), const glm::mat4& UTILS_UNUSED_PARAM(projection)) const {
        unsigned int diffuse_idx = 0;
        unsigned int specular_idx = 0;
        unsigned int normal_idx = 0;
        unsigned int height_idx = 0;
        for (unsigned int i = 0; i < m_textures.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            std::string number{};
            std::string name = m_textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuse_idx++);
            else if (name == "texture_specular")
                number = std::to_string(specular_idx++);
            else if (name == "texture_normal")
                number = std::to_string(normal_idx++);
            else if (name == "texture_height")
                number = std::to_string(height_idx++);

            shader_program.SetUniform((name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
        }

        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);

    }
}

#endif // !MESH_HPP