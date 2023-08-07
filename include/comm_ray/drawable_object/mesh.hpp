#pragma once
#ifndef _MESH_HPP_
#define _MESH_HPP_

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


}

#endif // !_MESH_HPP_