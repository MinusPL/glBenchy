#include "resource_manager.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include <yaml-cpp/yaml.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <iostream>

std::unordered_map<_UUID, Object*> ResourceManager::resources = std::unordered_map<_UUID, Object*>();

static _UUID  LoadMetadata(const char* filePath)
{
    _UUID uuidData;
    std::filesystem::path p(std::string(filePath)+".meta");
    if(std::filesystem::exists(p))
    {
        YAML::Node meta = YAML::LoadFile(p.string());
        uuidData.from_string(meta["guid"].as<std::string>());
    }
    else
    {
        uuidData = UUIDGenerator::GetUUID();
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "guid";
        out << YAML::Value << UUIDGenerator::UUIDToString(uuidData);
        out << YAML::EndMap;
        std::ofstream outMetaFile(p);
        outMetaFile << out.c_str();
        outMetaFile.close();
    }
    return uuidData;
}

Shader* ResourceManager::LoadShader(const char *vertexPath, const char *fragmentPath)
{
    _UUID objectID = LoadMetadata(fragmentPath);

    Shader* sh = new Shader();
    sh->guid = objectID;

    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;

    std::ifstream vertexShaderFile(vertexPath);
    std::ifstream fragmentShaderFile(fragmentPath);
    std::stringstream vertexStream, fragmentStream;

    vertexStream << vertexShaderFile.rdbuf();
    fragmentStream << fragmentShaderFile.rdbuf();
    // close file handlers
    vertexShaderFile.close();
    fragmentShaderFile.close();
    // Convert stream into string
    vertexCode = vertexStream.str();
    fragmentCode = fragmentStream.str();

    sh->Compile(vertexCode.c_str(), fragmentCode.c_str(), nullptr);
    ResourceManager::resources[sh->guid] = sh;
    return sh;
}

Material* ResourceManager::LoadMaterial(const char *materialFilePath)
{
    _UUID objectID = LoadMetadata(materialFilePath);
    Material* mat = new Material();
    mat->guid = objectID;
    YAML::Node meta = YAML::LoadFile(materialFilePath);
    mat->m_Name = meta["m_Name"].as<std::string>();
    
    return new Material();
}

Mesh* processMesh(aiMesh * mesh, const aiScene * scene)
{
    Mesh* model_mesh = new Mesh();
	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		model_mesh->vertices.push_back({mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z});
		model_mesh->normals.push_back({mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z});
        //mesh->mColors
		if(mesh->mTextureCoords[0])
		{
			model_mesh->uvs.push_back({mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
		}
		else
		{
			model_mesh->uvs.push_back({0.0f, 0.0f});
		}
		model_mesh->tangents.push_back({mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z});
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			model_mesh->indices.push_back(face.mIndices[j]);
	}

	//Process materials from loaded model
	// if (mesh->mMaterialIndex >= 0)
	// {
	// 	aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
	// 	Material* mat = new Material();
	// 	aiColor3D difColor, ambColor, specColor;
	// 	float shininess = 0.0f;
	// 	material->Get(AI_MATKEY_COLOR_DIFFUSE, difColor);
	// 	material->Get(AI_MATKEY_COLOR_AMBIENT, ambColor);
	// 	material->Get(AI_MATKEY_COLOR_SPECULAR, specColor);
	// 	material->Get(AI_MATKEY_SHININESS, shininess);
    //     material->
	// 	//mat->ambient = glm::vec3(ambColor.r, ambColor.g, ambColor.b);
	// 	mat->diffuse = glm::vec3(difColor.r, difColor.g, difColor.b);
	// 	//mat->diffuse = glm::vec3(1.0f,1.0f,1.0f);


	// 	mat->specular = glm::vec3(specColor.r, specColor.g, specColor.b);
	// 	mat->shiness = shininess;
	// 	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	// 	{
	// 		aiString texString;
	// 		material->GetTexture(aiTextureType_DIFFUSE, 0, &texString);
	// 		if (ResourceManager::GetTexture(texString.C_Str()) == nullptr)
	// 		{
	// 			std::string texture_path = this->directory + "/" + texString.C_Str();
	// 			ResourceManager::LoadTexture(texture_path.c_str(), texString.C_Str());
	// 		}
	// 		mat->diffuseTexture = ResourceManager::GetTexture(texString.C_Str());
	// 	}

	// 	if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
	// 	{
	// 		aiString texString;
	// 		material->GetTexture(aiTextureType_NORMALS, 0, &texString);
	// 		if (ResourceManager::GetTexture(texString.C_Str()) == nullptr)
	// 		{
	// 			std::string texture_path = this->directory + "/" + texString.C_Str();
	// 			ResourceManager::LoadTexture(texture_path.c_str(), texString.C_Str());
	// 		}
	// 		mat->normalMap = ResourceManager::GetTexture(texString.C_Str());
	// 	}

	// 	if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
	// 	{
	// 		aiString texString;
	// 		material->GetTexture(aiTextureType_SPECULAR, 0, &texString);
	// 		if (ResourceManager::GetTexture(texString.C_Str()) == nullptr)
	// 		{
	// 			std::string texture_path = this->directory + "/" + texString.C_Str();
	// 			ResourceManager::LoadTexture(texture_path.c_str(), texString.C_Str());
	// 		}
	// 		mat->specularTexture = ResourceManager::GetTexture(texString.C_Str());
	// 	}

	// 	materials.push_back(mat);
	// }
    return nullptr;
}

void processNode(aiNode * node, const aiScene * scene, GLBObject* rootObject)
{
	std::cout << "NODE: " << node->mName.C_Str() << std::endl;
    // process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{

		//Add all meshes to object.
        // the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::cout << "MESH: " << mesh->mName.C_Str() << " VERTS: " << mesh->mNumVertices << std::endl; 
		//meshes.push_back(processMesh(mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, rootObject);
	}
}

GLBObject* ResourceManager::LoadModel(const char *modelFilePath)
{
    _UUID objectID = LoadMetadata(modelFilePath);
    //Time for Assimp processing!
    Assimp::Importer import;
	const aiScene *scene = import.ReadFile(modelFilePath, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return nullptr;
	}

    GLBObject* objectToReturn = new GLBObject();

	processNode(scene->mRootNode, scene, objectToReturn);

    return objectToReturn;
}
