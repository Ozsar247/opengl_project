#pragma once

#include <glad/glad.h>
#include <vector>
#include <cstddef>

struct VertexAttrib {
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized;
    size_t offset;   // NEW: byte offset
};

class BufferRenderer {
public:
    BufferRenderer() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
    }

    void cleanup() const {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    void bind() const { glBindVertexArray(vao); }
    void unbind() const { glBindVertexArray(0); }

    template<typename T>
    void setVertices(const T* data, size_t count, GLenum usage = GL_STATIC_DRAW) {
        vertexCount = count;
        bind();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(T), data, usage);
    }

    template<typename T>
    void setVertices(const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW) {
        setVertices(data.data(), data.size(), usage);
    }

    template<typename T>
    void setIndices(const T* data, size_t count, GLenum usage = GL_STATIC_DRAW) {
        indexCount = count;
        hasEBO = true;
        bind();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(T), data, usage);
    }

    template<typename T>
    void setIndices(const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW) {
        setIndices(data.data(), data.size(), usage);
    }

    // --- AUTOMATIC MODE (unchanged)
    void addAttrib(GLuint index, GLint size, GLenum type, GLboolean normalized = GL_FALSE) {
        size_t offset = autoStride;
        attribs.push_back({index, size, type, normalized, offset});
        autoStride += size * typeSize(type);
    }

    // --- STRUCT MODE: manual offset
    void addAttribOffset(GLuint index, GLint size, GLenum type, size_t offset, GLboolean normalized = GL_FALSE) {
        attribs.push_back({index, size, type, normalized, offset});
    }

    // Set stride manually when using structs
    void setStride(size_t s) { totalStride = s; }

    // Link attributes
    void link() {
        bind();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        size_t stride = (totalStride == 0) ? autoStride : totalStride;

        for (auto& a : attribs) {
            glEnableVertexAttribArray(a.index);
            glVertexAttribPointer(
                a.index,
                a.size,
                a.type,
                a.normalized,
                stride,
                (void*)a.offset
            );
        }
        unbind();
    }

    void draw() const {
        bind();
        if (hasEBO) {
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        }
        unbind();
    }

private:
    GLuint vao = 0, vbo = 0, ebo = 0;

    std::vector<VertexAttrib> attribs;

    size_t autoStride = 0;     // tightly packed mode
    size_t totalStride = 0;    // struct mode

    size_t vertexCount = 0;
    size_t indexCount = 0;
    bool hasEBO = false;

    size_t typeSize(GLenum type) const {
        switch (type) {
            case GL_FLOAT: return sizeof(float);
            case GL_INT: return sizeof(int);
            case GL_UNSIGNED_INT: return sizeof(unsigned int);
            case GL_DOUBLE: return sizeof(double);
            case GL_BYTE: return sizeof(char);
            case GL_UNSIGNED_BYTE: return sizeof(unsigned char);
            case GL_SHORT: return sizeof(short);
            case GL_UNSIGNED_SHORT: return sizeof(unsigned short);
            default: return 0;
        }
    }
};
