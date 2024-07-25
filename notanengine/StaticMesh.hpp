#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <utility>
#include <vector>

#include "Material.hpp"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct VertexAttribProperties
{
    unsigned int Location = 0;
    unsigned int NumValues = 0;
    size_t Stride = 0;
    size_t Offset = 0;
};

struct MeshData
{
    MeshData(const std::vector<float>& vertexPositions, const std::vector<unsigned int>& indices, const std::vector<VertexAttribProperties>& vertexAttribs);
    MeshData(const std::vector<float>& vertexPositions, const std::vector<unsigned int>& indices, const std::vector<VertexAttribProperties>& vertexAttribs, const Material& mat);
    MeshData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const Material& material);

    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    unsigned int NumIndices = 0;
    Material Mat;
    bool UseMaterial = true;
};

class StaticMesh
{
public:
    StaticMesh();

    StaticMesh(const std::vector<MeshData>& subMeshes);

    StaticMesh(const std::vector<float>& vertexPositions, const std::vector<unsigned int>& indices, const std::vector<VertexAttribProperties>& vertexAttribs);
    StaticMesh(const std::vector<float>& vertexPositions, const std::vector<unsigned int>& indices, const std::vector<VertexAttribProperties>& vertexAttribs, const Material& mat);

    StaticMesh(const StaticMesh& other);
    StaticMesh(StaticMesh&& other);

    StaticMesh& operator=(const StaticMesh& other)
    {
	if (this != &other)
	{
	    m_meshData = other.m_meshData;
	}

	return *this;
    }

    StaticMesh& operator=(StaticMesh&& other)
    {
	if (this != &other)
	{
	    m_meshData = std::move(other.m_meshData);
	}

	return *this;
    }

    inline std::vector<MeshData>& GetSubMeshesRef() noexcept { return m_meshData; }

    void Draw() const noexcept;

private:
    std::vector<MeshData> m_meshData;
};

