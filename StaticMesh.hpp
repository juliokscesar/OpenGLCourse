#pragma once

#include <glad/glad.h>


class StaticMesh
{
public:
    StaticMesh()
        : m_VBO(0), m_VAO(0), m_EBO(0), m_vertices(0), m_useIndexedDrawing(false) {}

    StaticMesh(float verticesData[], GLsizeiptr verticesSize, GLuint numVertices);
    StaticMesh(float verticesData[], GLsizeiptr verticesSize, GLuint numVertices, GLuint indices[], GLsizeiptr indSize);

    StaticMesh(const StaticMesh& other);

    StaticMesh& operator=(const StaticMesh& other)
    {
        if (this != &other)
        {
            this->m_VBO = other.m_VBO;
            this->m_VAO = other.m_VAO;
            this->m_vertices = other.m_vertices;
            this->m_useIndexedDrawing = other.m_useIndexedDrawing;
        }

        return *this;
    }

    void SetVertexAttribute(GLuint loc, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* offsetSize) const noexcept;
    void SetEnabledVertexAttribute(GLuint loc, bool enable) const noexcept;

    void Draw() const noexcept;


private:
    GLuint m_VBO, m_VAO;
    GLuint m_EBO;
    GLuint m_vertices;

    bool m_useIndexedDrawing;
};
