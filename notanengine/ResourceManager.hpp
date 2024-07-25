#pragma once

#include <vector>
#include <array>
#include <unordered_map>

#include <glm/glm.hpp>
#include <assimp/scene.h>

#include "Shader.hpp"
#include "StaticMesh.hpp"
#include "Entity.hpp"

//////////////////////////////////
/// ASSIMP LOADING FUNCTIONS
//////////////////////////////////

namespace ResourceManager
{
    void InitializeLocations();

    Shader LoadShader(const std::string& vertexPath, const std::string& fragPath);

    Texture2D LoadTextureFromFile(const std::string& path);

    struct Model
    {
        StaticMesh Mesh;
        std::string Path;
    };

    Model LoadModel(const std::string& path);

    void ProcessAssimpNode(aiNode* node, const aiScene* scene, std::vector<MeshData>& meshBuffer);

    MeshData ProcessAssimpMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture2D> LoadMaterialTextures(aiMaterial* mat, aiTextureType type);
}

//////////////////////////////////
