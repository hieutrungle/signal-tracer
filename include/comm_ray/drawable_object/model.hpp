#pragma once
#ifndef _MODEL_HPP_
#define _MODEL_HPP_

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "cyGL.h"
#include "drawable.hpp"
#include "mesh.hpp"
#include <string>

namespace SignalTracer {
    class Model : public Drawable {
    public:
        Model(const std::string& path) {
            load_model(path);
        }

        void draw(cy::GLSLProgram& shader_program, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const override;

        static void load_texture(const char* path, unsigned int& texture);

        const std::vector<Mesh>& get_meshes() const { return m_meshes; }
        const std::vector<Texture>& get_textures_loaded() const { return m_textures_loaded; }
        std::string get_directory() const { return m_directory; }
        void transform(const glm::mat4& model_mat);

        void setup_draw() const override;

    private:
        std::vector<Mesh> m_meshes{};
        std::string m_directory{};
        std::vector<Texture> m_textures_loaded;

        void load_model(const std::string& path);
        void process_node(aiNode* node, const aiScene* scene);
        Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type, const std::string& type_name);
    };
}


#endif // !_MODEL_HPP_