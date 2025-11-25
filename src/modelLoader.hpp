#pragma once

#include <glm/glm.hpp>
#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

class ModelLoader {
public:
    ModelLoader(const char* path) {
        loadModel(path);
    }

    void Draw(Shader &shader) {
        for (auto& mesh : meshes)
            mesh.Draw(shader);
    }  

private:
    std::vector<Texture*> textures_loaded; // store pointers to avoid duplicates
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(const std::string& path) {
        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);	

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
            return;
        }

        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<MeshTexture> textures;

        // Process vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex{};
            glm::vec3 vector;

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;

                if (mesh->HasTangentsAndBitangents()) {
                    vector.x = mesh->mTangents[i].x;
                    vector.y = mesh->mTangents[i].y;
                    vector.z = mesh->mTangents[i].z;
                    vertex.Tangent = vector;

                    vector.x = mesh->mBitangents[i].x;
                    vector.y = mesh->mBitangents[i].y;
                    vector.z = mesh->mBitangents[i].z;
                    vertex.Bitangent = vector;
                } else {
                    vertex.Tangent = glm::vec3(0.0f);
                    vertex.Bitangent = glm::vec3(0.0f);
                }
            } else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        // Process indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // Process materials
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", textures);
            loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", textures);
            loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", textures);
            loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", textures);
        }

        return Mesh(vertices, indices, textures);
    }

    void loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, std::vector<MeshTexture>& outTextures) {
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            std::string texPath = directory + "/" + str.C_Str();

            // Check if texture was already loaded
            auto it = std::find_if(textures_loaded.begin(), textures_loaded.end(),
                [&](Texture* t){ return t->path == texPath; });

            Texture* tex;
            if (it != textures_loaded.end()) {
                tex = *it; // reuse existing
            } else {
                tex = new Texture(texPath.c_str());
                textures_loaded.push_back(tex);
            }

            outTextures.push_back({ tex, typeName });
        }
    }
};
