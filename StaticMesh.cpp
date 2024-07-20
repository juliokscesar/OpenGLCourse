#include "StaticMesh.hpp"

StaticMesh::StaticMesh(float verticesData[], GLsizeiptr verticesSize, GLuint numVertices)
{
    m_useIndexedDrawing = false;
    m_vertices = numVertices;

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, verticesSize, verticesData, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

StaticMesh::StaticMesh(float verticesData[], GLsizeiptr verticesSize, GLuint numVertices, GLuint indices[], GLsizeiptr indSize)
{
    m_useIndexedDrawing = true;
    m_vertices = numVertices;

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, verticesSize, verticesData, GL_STATIC_DRAW);

    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indSize, indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


}

StaticMesh::StaticMesh(const StaticMesh &other)
{
    this->m_VAO = other.m_VAO;
    this->m_VBO = other.m_VBO;
    this->m_EBO = other.m_EBO;
    this->m_vertices = other.m_vertices;
    this->m_useIndexedDrawing = other.m_useIndexedDrawing;
}

StaticMesh::StaticMesh(StaticMesh&& other)
{
    *this = std::move(other);
}

void StaticMesh::SetVertexAttribute(GLuint loc, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *offsetSize) const noexcept
{
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    if (m_useIndexedDrawing)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    glVertexAttribPointer(loc, size, type, normalized, stride, offsetSize);
    glEnableVertexAttribArray(loc);
}

void StaticMesh::SetEnabledVertexAttribute(GLuint loc, bool enable) const noexcept
{
    if (enable)
        glEnableVertexAttribArray(loc);
    else
        glDisableVertexAttribArray(loc);
}

void StaticMesh::Draw() const noexcept
{
    glBindVertexArray(m_VAO);

    if (m_useIndexedDrawing)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glDrawElements(GL_TRIANGLES, m_vertices, GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, m_vertices);
    }
}
