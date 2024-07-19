#include "old_ModelFactory.hpp"
#include "assimp/material.h"

#include <cstddef>
#include <cstring>
#include <iostream>

#include <glad/glad.h>

#ifndef STB_IMAGE_IMPLEMENTATION
    #define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb/stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


static std::vector<Texture> g_loadedTextures;

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
    std::string filename(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    if (!data)
    {
	std::cerr << "Texture failed to load at path: " << path << '\n';
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

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}


Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
    : Vertices(vertices), Indices(indices), Textures(textures)
{
    setupMesh();
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    // VBO initialization
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    // since C++ structs memory layout is sequential, we can
    // calculate the size based on the size of the struct 'Vertex'
    // since it's composed of 2 vec3 and 1 vec2 floats, thus a
    // sequential layout of 2*3 + 1*2 = 8 floats = 32 bytes
    // and then we just multiply it by the number of vertices
    const size_t vboSize = Vertices.size() * sizeof(Vertex);
    glBufferData(GL_ARRAY_BUFFER, vboSize, Vertices.data(), GL_STATIC_DRAW);

    constexpr size_t A_STRIDE = sizeof(Vertex);
    // position vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, A_STRIDE, (void*)0);
    glEnableVertexAttribArray(0);

    // normals vertex attribute
    // PS: offsetof is a preprocessor directive defined in
    // <cstddef> that calulates the offset in bytes from the
    // beginning of an object to its member
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, A_STRIDE, (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    // texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, A_STRIDE, (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);


    // EBO initialization
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    const size_t indicesSize = Indices.size() * sizeof(unsigned int);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, Indices.data(), GL_STATIC_DRAW);
}

void Mesh::Draw(const Shader& shader)
{
    // draw assuming we're using a sampler2D array in our shader
    // we specify the MAX_NUMBER_SAMPLER2D here and in our shader to be the same
    constexpr unsigned int MAX_NUMBER_SAMPLER2D = 15;

    size_t texturesSize = Textures.size();
    if (texturesSize > MAX_NUMBER_SAMPLER2D)
    {
	std::cout << "Textures of size " << texturesSize << " exceeds MAX_NUMBER_SAMPLER2D\n";
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
	shader.SetInt(uniformTex.c_str(), i);
	glBindTexture(GL_TEXTURE_2D, Textures[i].Id);
    }

    glActiveTexture(GL_TEXTURE0);

    // finally draw
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
}


Model::Model(const std::string& path)
{
    loadModel(path);
}

void Model::Draw(const Shader& shader)
{
    for (size_t i = 0; i < m_meshes.size(); i++)
	m_meshes[i].Draw(shader);
}

void Model::loadModel(const std::string& path)
{
    std::cout << "Started loading model " << path << '\n';
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
	std::cout << "loadModel() Assimp error when loading " << path << ": " << importer.GetErrorString() << '\n';
	return;
    }

    m_directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);

    std::cout << "Finished loading model " << path << "\n";
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process all node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
	// get mesh based on the index in mMeshes of the node
	aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
	// build our mesh and add to this model
	m_meshes.push_back(processMesh(mesh, scene));
    }
    // then recursively do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
	processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // process each mesh vertex
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
	Vertex vertex{};

	// process mesh vertices
	glm::vec3 vector;
	vector.x = mesh->mVertices[i].x;
	vector.y = mesh->mVertices[i].y;
	vector.z = mesh->mVertices[i].z;
	vertex.Position = vector;

	// process mesh normals
	vector.x = mesh->mNormals[i].x;
	vector.y = mesh->mNormals[i].y;
	vector.z = mesh->mNormals[i].z;
	vertex.Normal = vector;

	// process texture coordinates
	// assimp allows models to have up to 8 different TexCoords
	// per vertex. but (for some reason) we only care about 1
	if (mesh->mTextureCoords[0]) // check if has TexCoords
	{
	    glm::vec2 vec;
	    vec.x = mesh->mTextureCoords[0][i].x;
	    vec.y = mesh->mTextureCoords[0][i].y;
	    vertex.TexCoords = vec;
	}
	else 
	{
	    vertex.TexCoords = glm::vec2(0.0f, 0.0f);
	}

	vertices.push_back(vertex);
    }

    // process each mesh index
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
	aiFace face = mesh->mFaces[i];
	for (unsigned int j = 0; j < face.mNumIndices; j++)
	    indices.push_back(face.mIndices[j]);
    }

    // process mesh materials
    if (mesh->mMaterialIndex >= 0)
    {
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	// first take diffuse maps
	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);

	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	// now take specular maps
	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
	aiString str;
	mat->GetTexture(type, i, &str);

	bool skip = false;
	for (unsigned int j = 0; j < g_loadedTextures.size(); j++)
	{
	    if (std::strcmp(g_loadedTextures[j].Path.c_str(), str.C_Str()) == 0)
	    {
		textures.push_back(g_loadedTextures[j]);
		skip = true;
		break;
	    }
	}
	if (skip)
	    continue;

	Texture texture;
	texture.Id = TextureFromFile(str.C_Str(), m_directory);
	
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
	}
	
	texture.Path = str.C_Str();

	textures.push_back(texture);
	g_loadedTextures.push_back(texture);
    }

    return textures;
}

