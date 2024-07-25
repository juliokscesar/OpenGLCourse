#pragma once

#include <glm/glm.hpp>

#include <assimp/scene.h>

#include "StaticMesh.hpp"

//////////////////////////////////
/// ASSIMP LOADING FUNCTIONS
//////////////////////////////////

namespace ResourceManager
{
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
