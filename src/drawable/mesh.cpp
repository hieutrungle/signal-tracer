#include "mesh.hpp"

namespace SignalTracer {

    void Mesh::transform(const glm::mat4& model_mat) {
        m_model_mat = model_mat;
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