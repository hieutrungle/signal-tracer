#include "model.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


namespace SignalTracer {

    void Model::draw(cy::GLSLProgram& shader_program, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const {
        shader_program.Bind();
        glm::mat3 normal_matrix{ glm::mat3(glm::transpose(glm::inverse(view * model))) };
        shader_program.SetUniformMatrix4("model_view", glm::value_ptr(view * model), 1, false);
        shader_program.SetUniformMatrix4("model_view_projection", glm::value_ptr(projection * view * model), 1, false);
        shader_program.SetUniformMatrix3("normal_matrix", glm::value_ptr(normal_matrix), 1, false);
        for (auto& mesh : m_meshes)
            mesh.draw(shader_program, model, view, projection);
    }

    void Model::setup_draw() const {
        for (auto& mesh : m_meshes)
            mesh.setup_draw();
    }

    void Model::load_model(const std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        m_directory = path.substr(0, path.find_last_of('/'));
        std::cout << "Model directory: " << m_directory << std::endl;
        process_node(scene->mRootNode, scene);
    }

    void Model::process_node(aiNode* node, const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            m_meshes.push_back(
                process_mesh(
                    scene->mMeshes[node->mMeshes[i]],
                    scene
                )
            );
        }
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
            process_node(node->mChildren[i], scene);
    }

    Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices{};
        std::vector<unsigned int> indices{};
        std::vector<Texture> textures{};

        // process vertex positions, normals and texture coordinates
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            Vertex vertex{};
            vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

            if (mesh->mTextureCoords[0]) {
                vertex.tex_coords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            else
                vertex.tex_coords = glm::vec2(0.0f, 0.0f);
            vertices.push_back(vertex);
        }

        // process indices
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }

        // process material
        if (mesh->mMaterialIndex > 0) {
            // std::cout << "mesh->mMaterialIndex: " << mesh->mMaterialIndex << std::endl;
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            // std::cout << "material->GetName(): " << material->GetName().C_Str() << std::endl;

            std::vector<Texture> diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

            std::vector<Texture> specular_maps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

            std::vector<Texture> normal_maps = load_material_textures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());

            std::vector<Texture> height_maps = load_material_textures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), height_maps.begin(), height_maps.end());
        }

        return Mesh(vertices, indices, textures);
    }

    std::vector<Texture> Model::load_material_textures(aiMaterial* mat, aiTextureType type, const std::string& type_name) {
        std::vector<Texture> textures{};
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString texture_name{};
            mat->GetTexture(type, i, &texture_name);
            std::string path = m_directory + '/' + texture_name.C_Str();
            bool skip = false;
            for (auto& texture_loaded : m_textures_loaded) {
                if (std::strcmp(texture_loaded.path.data(), path.c_str()) == 0) {
                    textures.push_back(texture_loaded);
                    skip = true;
                    break;
                }
            }

            if (!skip) {
                std::cout << "Loading texture: " << path << std::endl;
                Texture texture{};
                texture.id = 0;
                load_texture(path.c_str(), texture.id);
                texture.type = type_name;
                texture.path = path;
                textures.push_back(texture);
                m_textures_loaded.push_back(texture);
            }
        }
        return textures;
    }

    void Model::load_texture(const char* path, unsigned int& texture) {

        stbi_set_flip_vertically_on_load(true);

        glGenTextures(1, &texture);
        int width, height, num_channels;
        unsigned char* data = stbi_load(path, &width, &height, &num_channels, 0);
        if (data) {
            GLenum format{ GL_RGB };
            if (num_channels == 1)
                format = GL_RED;
            else if (num_channels == 3)
                format = GL_RGB;
            else if (num_channels == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, texture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cerr << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }

    void Model::transform(const glm::mat4& model_mat) {
        m_model_mat = model_mat;
        for (auto& mesh : m_meshes)
            mesh.transform(model_mat);
    }
}