#pragma once

#include <glm/glm.hpp>

#include <assimp/scene.h>

#include <vector>

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
    std::string Path; 
};

unsigned int LoadTextureFromFile(const std::string& path);

///////////////////////////////


struct Material
{
    std::vector<Texture> DiffuseMaps;
    std::vector<Texture> SpecularMaps;
    float Shininess;
};


///////////////////////////////
/// MESH & MODEL DECLARATIONS
///////////////////////////////

class Mesh
{
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const Material& material);

    Mesh(const Mesh& other)
	:
	Vertices(other.Vertices),
	Indices(other.Indices),
	Mat(other.Mat),
	m_VAO(other.m_VAO), 
	m_VBO(other.m_VBO), 
	m_EBO(other.m_EBO),
	m_numIndices(other.m_numIndices) {}

    Mesh(Mesh&& other)
	: 
	Vertices(std::move(other.Vertices)),
	Indices(std::move(other.Indices)),
	Mat(std::move(other.Mat)),
	m_VAO(std::move(other.m_VAO)), 
	m_VBO(std::move(other.m_VBO)), 
	m_EBO(std::move(other.m_EBO)),
	m_numIndices(std::move(other.m_numIndices)) {}

    Mesh& operator=(const Mesh& other)
    {
	if (this != &other)
	{
	    this->Vertices = other.Vertices;
	    this->Indices = other.Indices;
	    this->Mat = other.Mat;

	    this->m_VAO = other.m_VAO;
	    this->m_VBO = other.m_VBO;
	    this->m_EBO = other.m_EBO;
	    this->m_numIndices = other.m_numIndices;
	}

	return *this;
    }

    Mesh& operator=(Mesh&& other)
    {
	if (this != &other)
	{
	    this->Vertices = std::move(other.Vertices);
	    this->Indices = std::move(other.Indices);
	    this->Mat = std::move(other.Mat);

	    this->m_VAO = std::move(other.m_VAO);
	    this->m_VBO = std::move(other.m_VBO);
	    this->m_EBO = std::move(other.m_EBO);
	    this->m_numIndices = std::move(other.m_numIndices);
	}

	return *this;
    }

    void Draw() const noexcept;

private:
    void setupRenderData();

public:
    // Loaded mesh data
    std::vector<Vertex>		Vertices;
    std::vector<unsigned int>	Indices;
    Material			Mat;

private:
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;
    unsigned int m_numIndices = 0;
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

    Model& operator=(const Model& other)
    {
	if (this != &other)
	{
	    this->m_meshes = other.m_meshes;
	    this->m_path   = other.m_path;
	}

	return *this;
    }

    Model& operator=(Model&& other)
    {
	if (this != &other)
	{
	    this->m_meshes = std::move(other.m_meshes);
	    this->m_path   = std::move(other.m_path);
	}

	return *this;
    }

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
    Model LoadModel(const std::string& path);
    
    void ProcessAssimpNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshBuffer);

    Mesh ProcessAssimpMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type);
}

//////////////////////////////////
