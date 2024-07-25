#include "ResourceManager.hpp"

#include <glad/glad.h>

#include <iostream>
#include <unordered_map>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/material.h>

static std::string formatPath(const std::string& p)
{
    std::string out = p;
    auto pos = out.find("\\");
    while (pos != std::string::npos)
    {
	out.replace(pos, 1, "/");
	pos = out.find("\\", pos);
    }
    return out;
}

static std::string recursiveSearchForFile(const std::string& path, const std::string& desiredFileName)
{
    std::string formatted = formatPath(desiredFileName);
    auto desiredFile = std::filesystem::path(formatted).filename();

    for (const auto& p : std::filesystem::recursive_directory_iterator(std::filesystem::path(path)))
    {
		if (std::filesystem::is_directory(p))
			continue;

		const std::string foundFile = p.path().filename().string();
		if (foundFile.compare(desiredFile.string()) == 0)
			return std::string(p.path().string());
    }

    return "";
}

static std::string recursiveSearchForFolder(const std::string& path, const std::string& desiredPath)
{
	std::string formatted = formatPath(desiredPath);
	for (const auto& p : std::filesystem::recursive_directory_iterator(std::filesystem::path(path)))
    {
		if (!std::filesystem::is_directory(p))
			continue;

		const std::string foundFolder = p.path().filename().string();
		if (foundFolder.compare(formatted) == 0)
			return formatPath(std::string(p.path().string()));
    }

	return "";
}

static std::string g_assetsFullPath;
void checkCurrentPath()
{
	if (std::filesystem::current_path().string().compare(g_assetsFullPath) != 0)
		std::filesystem::current_path(g_assetsFullPath);
}


namespace ResourceManager
{
    void InitializeLocations()
    {
		std::filesystem::path currentPath = std::filesystem::current_path().string();
		// keep going back until folder reaches root folder 'notanengine'
		while (currentPath.filename() != "notanengine")
			currentPath = currentPath.parent_path();

		g_assetsFullPath = recursiveSearchForFolder(currentPath.string(), "assets");
		std::cout << "Updating g_assetsFullPath = " << g_assetsFullPath << '\n';
    }

    Shader LoadShader(const std::string &vertexPath, const std::string &fragPath)
    {
		checkCurrentPath();
        return Shader(g_assetsFullPath + "/" + formatPath(vertexPath), g_assetsFullPath + "/" + formatPath(fragPath));
    }

    Texture2D LoadTextureFromFile(const std::string &path)
	{
		checkCurrentPath();

		std::string fullPath = recursiveSearchForFile(".", path);

		int width, height, nrChannels;
		unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);

		if (!data)
		{
			std::cerr << "Texture to failed to load from path given " << path << " using fullPath found " << fullPath << '\n';
			stbi_image_free(data);
			return Texture2D();
		}

		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		Texture2DProperties texProps{width, height, format, path};

		Texture2D texture(data, texProps);
		stbi_image_free(data);
		return texture;
	}



	Model LoadModel(const std::string &path)
	{	
		checkCurrentPath();

		Model out{};

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, 
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_FlipUVs | 
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenNormals |
			aiProcess_FixInfacingNormals
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cerr <<"LoadModel assimp error when loading " << path << ": " << importer.GetErrorString() << '\n';
		}
    
		out.Mesh.GetSubMeshesRef().reserve(scene->mNumMeshes);
		out.Path = path;

		ProcessAssimpNode(scene->mRootNode, scene, out.Mesh.GetSubMeshesRef());

		return out;
	}


    void ProcessAssimpNode(aiNode *node, const aiScene *scene, std::vector<MeshData> &meshBuffer)
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

	MeshData ProcessAssimpMesh(aiMesh *mesh, const aiScene *scene)
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
		Material meshMaterial;

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

			// Take diffuse maps
			meshMaterial.DiffuseMaps = LoadMaterialTextures(mat, aiTextureType_DIFFUSE);

			// now take specular maps
			meshMaterial.SpecularMaps = LoadMaterialTextures(mat, aiTextureType_SPECULAR);

			// and finally material shininess
			float shininess; 
			if (aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shininess) != AI_SUCCESS)
				shininess = 20.0f; // set default value if cant get shininess

			meshMaterial.Shininess = shininess;
		}

		return MeshData(vertices, indices, meshMaterial);
	}

	std::vector<Texture2D> LoadMaterialTextures(aiMaterial *mat, aiTextureType type)
	{
		static std::unordered_map<std::string, Texture2D> s_loadedTextures;

		std::vector<Texture2D> textures;
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

			Texture2D texture = LoadTextureFromFile(path);
			if (texture.GetID() == 0)
				continue;

			textures.push_back(texture);
			s_loadedTextures[path] = texture;
		}

		return textures;
	}
}
