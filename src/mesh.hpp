#pragma once

#include <glm/glm.hpp>

#include "texture.hpp"
#include "bufferRenderer.hpp"

#include <stddef.h>


#define MAX_BONE_INFLUENCE 4


struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};


struct Tex {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Tex> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Tex> textures)
        : vertices(vertices), indices(indices), textures(textures) {
        setupMesh();
    };
    void Draw(Shader &shader) {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);

            std::string number;
            std::string name = textures[i].type;
            if (name == "texture_diffuse") {
                number = std::to_string(diffuseNr++);
            } else {
                number = std::to_string(specularNr++);
            }

            shader.setInt(("material." + name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);

        buf.draw();
    };
private:
    BufferRenderer buf;

    void setupMesh() {
        // Upload vertex + index buffers
        buf.setVertices(vertices);
        buf.setIndices(indices);

        // Struct stride
        buf.setStride(sizeof(Vertex));

        // ---- Vertex Attributes ----
        buf.addAttribOffset(0, 3, GL_FLOAT,  offsetof(Vertex, Position));     // vec3
        buf.addAttribOffset(1, 3, GL_FLOAT,  offsetof(Vertex, Normal));       // vec3
        buf.addAttribOffset(2, 2, GL_FLOAT,  offsetof(Vertex, TexCoords));    // vec2
        buf.addAttribOffset(3, 3, GL_FLOAT,  offsetof(Vertex, Tangent));      // vec3
        buf.addAttribOffset(4, 3, GL_FLOAT,  offsetof(Vertex, Bitangent));    // vec3

        // bone IDs → int[4] → use GL_INT
        buf.addAttribOffset(5, 4, GL_INT,    offsetof(Vertex, m_BoneIDs));    // ivec4

        // weights → float[4]
        buf.addAttribOffset(6, 4, GL_FLOAT,  offsetof(Vertex, m_Weights));    // vec4

        // Build VAO
        buf.link();
    }

};

