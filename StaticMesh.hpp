#pragma once

#include <glad/glad.h>

#include "Shader.hpp"

class StaticMesh
{
public:
    StaticMesh()
        : m_VBO(0), m_VAO(0), m_EBO(0), m_vertices(0), m_useIndexedDrawing(false), m_shader() {}

    StaticMesh(float verticesData[], GLsizeiptr verticesSize, GLuint numVertices, const std::string& vertexPath, const std::string& fragPath);
    StaticMesh(float verticesData[], GLsizeiptr verticesSize, GLuint numVertices, GLuint indices[], GLsizeiptr indSize, const std::string& vertexPath, const std::string& fragPath);

    StaticMesh(const StaticMesh& other);

    void SetVertexAttribute(GLuint loc, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* offsetSize) const noexcept;
    void SetEnabledVertexAttribute(GLuint loc, bool enable) const noexcept;

    Shader& GetShader() noexcept;
    void SetShader(const std::string& vertexPath, const std::string& fragPath);

    void Draw() const noexcept;


private:
    GLuint m_VBO, m_VAO;
    GLuint m_EBO;
    GLuint m_vertices;

    bool m_useIndexedDrawing;

    Shader m_shader;
};
