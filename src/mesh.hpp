#pragma once

#include <vector>
#include <string>
#include <stddef.h>

#include <glm/glm.hpp>

#include "shader.hpp"
#include "texture.hpp"
#include "bufferRenderer.hpp"  // make sure this is included

#ifndef MAX_BONE_INFLUENCE
#define MAX_BONE_INFLUENCE 4
#endif

// struct Vertex {
//     glm::vec3 Position;
//     glm::vec3 Normal;
//     glm::vec2 TexCoords;
//     glm::vec3 Tangent;
//     glm::vec3 Bitangent;
//     int m_BoneIDs[MAX_BONE_INFLUENCE];
//     float m_Weights[MAX_BONE_INFLUENCE];
// };

// Optional: struct for storing textures
struct MeshTexture {
    Texture* texture;
    std::string type; // "texture_diffuse", "texture_specular", etc.
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<MeshTexture> textures;

    Mesh(std::vector<Vertex> vertices,
         std::vector<unsigned int> indices,
         std::vector<MeshTexture> textures)
        : vertices(vertices), indices(indices), textures(textures)
    {
        setupMesh();
    }

    void Draw(Shader &shader) {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;

        for (unsigned int i = 0; i < textures.size(); i++) {
            MeshTexture &mt = textures[i];

            glActiveTexture(GL_TEXTURE0 + i);
            std::string number;
            if (mt.type == "texture_diffuse") number = std::to_string(diffuseNr++);
            else if (mt.type == "texture_specular") number = std::to_string(specularNr++);

            shader.setInt(("material." + mt.type + number).c_str(), i);
            mt.texture->bind(i);
        }

        glActiveTexture(GL_TEXTURE0);
        buf.draw();
    }

    unsigned int* GetVAO() { return buf.GetVAO(); }
    unsigned int* GetVBO() { return buf.GetVBO(); }
    unsigned int* GetEBO() { return buf.GetEBO() ? buf.GetEBO() : nullptr; }

private:
    BufferRenderer buf;

    void setupMesh() {
        buf.setVertices(vertices);
        buf.setIndices(indices);
        buf.setStride(sizeof(Vertex));

        buf.addAttribOffset(0, 3, GL_FLOAT, offsetof(Vertex, Position));
        buf.addAttribOffset(1, 3, GL_FLOAT, offsetof(Vertex, Normal));
        buf.addAttribOffset(2, 2, GL_FLOAT, offsetof(Vertex, TexCoords));
        buf.addAttribOffset(3, 3, GL_FLOAT, offsetof(Vertex, Tangent));
        buf.addAttribOffset(4, 3, GL_FLOAT, offsetof(Vertex, Bitangent));
        buf.addAttribOffset(5, 4, GL_INT, offsetof(Vertex, m_BoneIDs));
        buf.addAttribOffset(6, 4, GL_FLOAT, offsetof(Vertex, m_Weights));

        buf.link();
    }
};
