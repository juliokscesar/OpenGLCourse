#pragma once

#include <glad/glad.h>

#include "Shader.hpp"

class StaticMesh
{
public:
    StaticMesh(float verticesData[], GLsizeiptr verticesSize, GLuint numVertices, const std::string& vertexPath, const std::string& fragPath);
    StaticMesh(float verticesData[], GLsizeiptr verticesSize, GLuint numVertices, GLuint indices[], GLsizeiptr indSize, const std::string& vertexPath, const std::string& fragPath);

    StaticMesh(const StaticMesh& other);

    void SetVertexAttribute(GLuint loc, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* offsetSize) const noexcept;
    
    Shader& GetShader() noexcept;

    void Draw() const noexcept;


private:
    GLuint m_VBO, m_VAO;
    GLuint m_EBO;
    GLuint m_vertices;

    bool m_useIndexedDrawing;

    Shader m_shader;
};
