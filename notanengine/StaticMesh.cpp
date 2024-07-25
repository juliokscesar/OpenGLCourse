#include "StaticMesh.hpp"
#include <cstddef>


MeshData::MeshData(const std::vector<float>& vertexPositions, const std::vector<unsigned int>& indices, const std::vector<VertexAttribProperties>& vertexAttribs)
{
    UseMaterial = false;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    /// VBO SETUP
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    const size_t vboSize = vertexPositions.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, vboSize, vertexPositions.data(), GL_STATIC_DRAW);

    /// EBO SETUP
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    NumIndices = indices.size();
    const size_t eboSize = indices.size() * sizeof(unsigned int);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboSize, indices.data(), GL_STATIC_DRAW);

    /// Vertex Attributes
    for (const auto& attrib : vertexAttribs)
    {
	glVertexAttribPointer(
	    attrib.Location,
	    attrib.NumValues,
	    GL_FLOAT,
	    GL_FALSE,
	    attrib.Stride,
	    (void*)(attrib.Offset)
	);
	glEnableVertexAttribArray(attrib.Location);
    }
}

MeshData::MeshData(const std::vector<float>& vertexPositions, const std::vector<unsigned int>& indices, const std::vector<VertexAttribProperties>& vertexAttribs, const Material& mat)
{
    Mat = mat;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    /// VBO SETUP
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    const size_t vboSize = vertexPositions.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, vboSize, vertexPositions.data(), GL_STATIC_DRAW);

    /// EBO SETUP
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    NumIndices = indices.size();
    const size_t eboSize = indices.size() * sizeof(unsigned int);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboSize, indices.data(), GL_STATIC_DRAW);

    /// Vertex Attributes
    for (const auto& attrib : vertexAttribs)
    {
	glVertexAttribPointer(
	    attrib.Location,
	    attrib.NumValues,
	    GL_FLOAT,
	    GL_FALSE,
	    attrib.Stride,
	    (void*)(attrib.Offset)
	);
	glEnableVertexAttribArray(attrib.Location);
    }
}

MeshData::MeshData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const Material& material)
    : Mat(material)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    /// VBO SETUP
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    const size_t vboSize = vertices.size() * sizeof(Vertex);
    glBufferData(GL_ARRAY_BUFFER, vboSize, vertices.data(), GL_STATIC_DRAW);

    /// EBO SETUP
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    NumIndices = indices.size();
    const size_t eboSize = indices.size() * sizeof(unsigned int);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboSize, indices.data(), GL_STATIC_DRAW);

    /// Vertex Attributes SETUP
    constexpr size_t ATT_STRIDE = sizeof(Vertex);

    // position vertex attribute
    glVertexAttribPointer(
	0,
	3,
	GL_FLOAT,
	GL_FALSE,
	ATT_STRIDE,
	(void*)0
    );
    glEnableVertexAttribArray(0);

    // normal vertex attribute
    glVertexAttribPointer(
	1,
	3,
	GL_FLOAT,
	GL_FALSE,
	ATT_STRIDE,
	(void*)offsetof(Vertex, Normal)
    );
    glEnableVertexAttribArray(1);

    // tex coords vertex attribute
    glVertexAttribPointer(
	2,
	2,
	GL_FLOAT,
	GL_FALSE,
	ATT_STRIDE,
	(void*)offsetof(Vertex, TexCoords)
    );
    glEnableVertexAttribArray(2);
}

StaticMesh::StaticMesh()
{
}

StaticMesh::StaticMesh(const std::vector<MeshData>& subMeshes)
    : m_meshData(subMeshes)
{
}


StaticMesh::StaticMesh(const std::vector<float>& vertexPositions, const std::vector<unsigned int>& indices, const std::vector<VertexAttribProperties>& vertexAttribs)
{
    MeshData mesh(vertexPositions, indices, vertexAttribs);
    m_meshData.push_back(mesh);
}

StaticMesh::StaticMesh(const std::vector<float>& vertexPositions, const std::vector<unsigned int>& indices, const std::vector<VertexAttribProperties>& vertexAttribs, const Material& mat)
{
    MeshData mesh(vertexPositions, indices, vertexAttribs, mat);
    m_meshData.push_back(mesh);
}

StaticMesh::StaticMesh(const StaticMesh& other)
    : m_meshData(other.m_meshData)
{
}

StaticMesh::StaticMesh(StaticMesh&& other)
    : m_meshData(std::move(other.m_meshData))
{
}

void StaticMesh::Draw() const noexcept
{
    for (const auto& mesh : m_meshData)
    {
	glBindVertexArray(mesh.VAO);
	
	glDrawElements(GL_TRIANGLES, mesh.NumIndices, GL_UNSIGNED_INT, 0);
    }
}



