#pragma once

#include <glm/glm.hpp>

#include <assimp/scene.h>

#include <iostream>
#include <vector>

#include "Shader.hpp"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

//////////////////////////////
/// TEXTURE DECLARATIONS
//////////////////////////////

enum class TextureType
{
    DIFFUSE,
    SPECULAR
};

struct Texture
{
    unsigned int glID;
    TextureType Type;
    std::string Path; // TODO: change this to use std::hash
};

unsigned int LoadTextureFromFile(const std::string& path);

///////////////////////////////

///////////////////////////////
/// MESH & MODEL DECLARATIONS
///////////////////////////////

class Mesh
{
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);

    Mesh(const Mesh& other)
	:
	Vertices(other.Vertices),
	Indices(other.Indices),
	Textures(other.Textures),
	m_VAO(other.m_VAO), 
	m_VBO(other.m_VBO), 
	m_EBO(other.m_EBO) {}

    Mesh(Mesh&& other)
	: 
	Vertices(std::move(other.Vertices)),
	Indices(std::move(other.Indices)),
	Textures(std::move(other.Textures)),
	m_VAO(std::move(other.m_VAO)), 
	m_VBO(std::move(other.m_VBO)), 
	m_EBO(std::move(other.m_EBO)) {}

    Mesh& operator=(const Mesh& other)
    {
	if (this != &other)
	{
	    this->Vertices = other.Vertices;
	    this->Indices = other.Indices;
	    this->Textures = other.Textures;

	    this->m_VAO = other.m_VAO;
	    this->m_VBO = other.m_VBO;
	    this->m_EBO = other.m_EBO;
	}

	return *this;
    }

    Mesh& operator=(Mesh&& other)
    {
	if (this != &other)
	{
	    this->Vertices = std::move(other.Vertices);
	    this->Indices = std::move(other.Indices);
	    this->Textures = std::move(other.Textures);

	    this->m_VAO = std::move(other.m_VAO);
	    this->m_VBO = std::move(other.m_VBO);
	    this->m_EBO = std::move(other.m_EBO);
	}

	return *this;
    }

    void ActivateTexturesInShader(const Shader& shader);

    void Draw(const Shader& shader);

    inline void PrintInfo() const noexcept
    {
	std::cout << "VAO=" << m_VAO << " VBO=" << m_VBO << " EBO=" << m_EBO << " VerticesSize=" << Vertices.size() << " IndicesSize=" << Indices.size() << " TexturesSize=" << Textures.size() << '\n';
    }

private:
    void setupRenderData();

public:
    // Loaded mesh data
    std::vector<Vertex>		Vertices;
    std::vector<unsigned int>	Indices;
    std::vector<Texture>	Textures;

private:
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;
};

// A Model is an object that contains multiple meshes
class Model
{
public:
    Model() = default;

    Model(const std::vector<Mesh>& meshes, const std::string& path);
	
    Model(const Model& other)
	: m_meshes(other.m_meshes), m_path(other.m_path) {}

    Model(Model&& other)
	: m_meshes(std::move(other.m_meshes)), m_path(std::move(other.m_path)) {}

    void Draw(const Shader& shader);

    inline std::vector<Mesh>& GetMeshes() noexcept { return m_meshes; }

    inline std::string GetPath() const noexcept { return m_path; }
    inline void SetPath(const std::string& path) noexcept { m_path = path; }

private:
    std::vector<Mesh> m_meshes;
    std::string m_path;
};

//////////////////////////////////


//////////////////////////////////
/// ASSIMP LOADING FUNCTIONS
//////////////////////////////////

namespace ObjectLoader
{
    void LoadModel(const std::string& path, Model& outModel);
    
    void ProcessAssimpNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshBuffer);

    Mesh ProcessAssimpMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type);
}

//////////////////////////////////
