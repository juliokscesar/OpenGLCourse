#include "ModelFactory.hpp"

#include <glad/glad.h>

#include <iostream>
#include <cstddef>
#include <unordered_map>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/material.h>

static std::string recursiveSearchFullPath(const std::filesystem::path& path, const std::string& desiredFileName)
{
    for (const auto& p : std::filesystem::recursive_directory_iterator(path))
    {
	if (std::filesystem::is_directory(p))
	    continue;

	const std::string foundFile = p.path().filename();
	if (foundFile.compare(desiredFileName) == 0)
	    return std::string(p.path());
    }

    return "";
}

unsigned int LoadTextureFromFile(const std::string &path)
{
    std::string fullPath = recursiveSearchFullPath(".", path);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);

    if (!data)
    {
	std::cerr << "Texture to failed to load from path " << fullPath << '\n';
	stbi_image_free(data);
	return 0;
    }

    GLenum format;
    if (nrChannels == 1)
	format = GL_RED;
    else if (nrChannels == 3)
	format = GL_RGB;
    else if (nrChannels == 4)
	format = GL_RGBA;


    unsigned int textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(
	GL_TEXTURE_2D,
	0,
	format,
	width,
	height,
	0,
	format,
	GL_UNSIGNED_BYTE,
	data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    // wrap settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // filter settings (with mipmap for min)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return textureID;
}



Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
    : Vertices(vertices), Indices(indices), Textures(textures)
{
    setupRenderData();
}

void Mesh::setupRenderData()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    /// VBO initialization
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    
    const size_t vboSize = Vertices.size() * sizeof(Vertex);
    glBufferData(GL_ARRAY_BUFFER, vboSize, Vertices.data(), GL_STATIC_DRAW);
    
    /// EBO initialization
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    const size_t eboSize = Indices.size() * sizeof(unsigned int);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboSize, Indices.data(), GL_STATIC_DRAW);

    /// Vertex attributes setup
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

    // normals vertex attribute
    glVertexAttribPointer(
	    1, 
	    3, 
	    GL_FLOAT, 
	    GL_FALSE, 
	    ATT_STRIDE, 
	    (void*)offsetof(Vertex, Normal)
    );
    glEnableVertexAttribArray(1); 

    // texcoords vertex attribute
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

void Mesh::ActivateTexturesInShader(const Shader& shader)
{
    // Draw assuming we're using a sampler2D array in our shader
    // so we loop over every texture of our mesh (up to a max nr)
    // and set the sampler2D based on our index

    constexpr unsigned int MAX_NUMBER_SAMPLER2D = 15;

    const size_t texturesSize = Textures.size();
    if (texturesSize > MAX_NUMBER_SAMPLER2D)
    {
	std::cout << "Textures of size " << texturesSize << " exceeds MAX_NUMBER_SAMPLER2D of " << MAX_NUMBER_SAMPLER2D << '\n';
	return;
    }

    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (int i = 0; i < texturesSize; i++)
    {
	glActiveTexture(GL_TEXTURE0 + i);

	char texIndex;
	std::string texName;
	if (Textures[i].Type == TextureType::DIFFUSE)
	{
	    texIndex = (diffuseNr - 1) + '0';
	    diffuseNr++;
	    texName = "texture_diffuse";
	}
	else if (Textures[i].Type == TextureType::SPECULAR)
	{
	    texIndex = (specularNr - 1) + '0';
	    specularNr++;
	    texName = "texture_specular";
	}

	const std::string uniformTex = "u_material." + texName + '[' + texIndex + ']';
	shader.SetInt(uniformTex, i);
	glBindTexture(GL_TEXTURE_2D, Textures[i].glID);	
    }

    glActiveTexture(GL_TEXTURE0);

}

// TODO: take shader out of this function 
void Mesh::Draw(const Shader& shader)
{
    ActivateTexturesInShader(shader);

    glBindVertexArray(m_VAO);
    
    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
}



Model::Model(const std::vector<Mesh>& meshes, const std::string& path)
    : m_meshes(meshes), m_path(path)
{
}

void Model::Draw(const Shader& shader)
{
    for (size_t i = 0; i < m_meshes.size(); i++)
	m_meshes[i].Draw(shader);
}



void ObjectLoader::LoadModel(const std::string &path, Model& outModel)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, 
	aiProcess_Triangulate |
	aiProcess_FlipUVs | 
	aiProcess_JoinIdenticalVertices |
	aiProcess_GenNormals
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
	std::cerr <<"LoadModel assimp error when loading " << path << ": " << importer.GetErrorString() << '\n';
    }
    
    outModel.GetMeshes().reserve(scene->mNumMeshes);
    outModel.SetPath(path);

    ProcessAssimpNode(scene->mRootNode, scene, outModel.GetMeshes());
}

void ObjectLoader::ProcessAssimpNode(aiNode *node, const aiScene *scene, std::vector<Mesh> &meshBuffer)
{
    // Process all node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
	// get mesh based on the index in mMeshes of the node
	aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

	// then convert to our Mesh type and add to the buffer
	meshBuffer.push_back(ProcessAssimpMesh(mesh, scene));
    }

    // Then recursively do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
	ProcessAssimpNode(node->mChildren[i], scene, meshBuffer);
    }
}

Mesh ObjectLoader::ProcessAssimpMesh(aiMesh *mesh, const aiScene *scene)
{
    // First, process each mesh Vertex (posVertex, normal and texcoord)
    std::vector<Vertex> vertices;
    vertices.reserve(mesh->mNumVertices);
    
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
	Vertex vertex;
	glm::vec3 vec;
	
	// process mesh position vertices
	vec.x = mesh->mVertices[i].x;
	vec.y = mesh->mVertices[i].y;
	vec.z = mesh->mVertices[i].z;
	vertex.Position = vec;

	// process mesh normals
	vec.x = mesh->mNormals[i].x;
	vec.y = mesh->mNormals[i].y;
	vec.z = mesh->mNormals[i].z;
	vertex.Normal = vec;

	// process texture coordinates
	// assimp allows models to have up to 8 different TexCoords
	// but we only care about 1
	if (mesh->mTextureCoords[0])
	{
	    glm::vec2 tvec(
		mesh->mTextureCoords[0][i].x,
		mesh->mTextureCoords[0][i].y
	    );
	    vertex.TexCoords = tvec;
	}
	else
	{
	    vertex.TexCoords = glm::vec2(0.0f, 0.0f);
	}

	vertices.push_back(vertex);

    }

    /// Process mesh indices for indexed drawing
    // can't pre-reserve indices size because otherwise
    // we would need to loop through every face
    // BUT, if we assume every face is a triangle
    // then the total number of indices is simply
    // 3 * number of faces
    std::vector<unsigned int> indices;
    indices.reserve(3 * mesh->mNumFaces);

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
	const aiFace& face = mesh->mFaces[i];
	for (unsigned int j = 0; j < face.mNumIndices; j++)
	    indices.push_back(face.mIndices[j]);
    }

    /// Process mesh materials
    std::vector<Texture> textures;

    if (mesh->mMaterialIndex >= 0)
    {
	aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

	// Take diffuse maps
	std::vector<Texture> diffuseMaps = LoadMaterialTextures(mat, aiTextureType_DIFFUSE);

	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	// now take specular maps
	std::vector<Texture> specularMaps = LoadMaterialTextures(mat, aiTextureType_SPECULAR);

	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> ObjectLoader::LoadMaterialTextures(aiMaterial *mat, aiTextureType type)
{
    static std::unordered_map<std::string, Texture> s_loadedTextures;

    std::vector<Texture> textures;
    const unsigned int textureCount = mat->GetTextureCount(type);
    
    for (unsigned int i = 0; i < textureCount; i++)
    {
	aiString aipath;
	mat->GetTexture(type, i, &aipath);
	std::string path(aipath.C_Str());

	// check if texture was loaded already
	if (auto tex = s_loadedTextures.find(path); tex != s_loadedTextures.end())
	{
	    textures.push_back(tex->second);
	    continue;
	}

	Texture texture;
	texture.glID = LoadTextureFromFile(path);
	
	switch (type)
	{
	case (aiTextureType_DIFFUSE):
	    texture.Type = TextureType::DIFFUSE;
	    break;
	
	case (aiTextureType_SPECULAR):
	    texture.Type = TextureType::SPECULAR;
	    break;
	
	// just to get rid of warnings
	default:
	    texture.Type = TextureType::DIFFUSE;
	    break;
	}

	texture.Path = aipath.C_Str();

	textures.push_back(texture);
	s_loadedTextures[path] = texture;
    }

    return textures;
}

