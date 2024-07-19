#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <assimp/scene.h>

#include "Shader.hpp"
#include "assimp/material.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

enum class TextureType
{
    DIFFUSE,
    SPECULAR
};

struct Texture
{
    unsigned int Id;
    TextureType Type;
    std::string Path;
};

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Mesh
{
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);

    void Draw(const Shader& shader);

public:
    // Loaded mesh data
    std::vector<Vertex>		Vertices;
    std::vector<unsigned int>	Indices;
    std::vector<Texture>	Textures;

private:
    unsigned int m_VAO, m_VBO, m_EBO;

    void setupMesh();
};


// A model contains multiple meshes (and possibly multiple textures)
class Model
{
public:
    Model(const std::string& path);

    void Draw(const Shader& shader);

private:
    void loadModel(const std::string& path);

    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type);

private:
    std::vector<Mesh> m_meshes;

    std::string m_directory;
};
