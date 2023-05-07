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

#include "../../stb_image/stb_image.h"

#include "../../components/mesh_renderer/mesh_renderer.h"

std::unordered_map<_UUID, Object*> ResourceManager::resources = std::unordered_map<_UUID, Object*>();

Shader* ResourceManager::defaultShader;
Material* ResourceManager::defaultMaterial;

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

static std::map<std::string, std::string> LoadModelMetadata(const char* filePath)
{
	//Load material overrides!
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

//Insert helper functions for yaml processing?


Material* ResourceManager::LoadMaterial(const char *materialFilePath)
{
    _UUID objectID = LoadMetadata(materialFilePath);
    Material* mat = new Material();
    mat->guid = objectID;
    YAML::Node fileData = YAML::LoadFile(materialFilePath);

	if(fileData["Material"] == nullptr) return nullptr;
	
	YAML::Node materialData = fileData["Material"];

	for(YAML::const_iterator it=materialData.begin(); it!=materialData.end(); it++) 
	{
		if(it->first.as<std::string>() == "m_Colors")
		{
			YAML::Node colors = it->second;
			for(YAML::const_iterator cit=colors.begin(); cit!=colors.end(); cit++) 
			{
				std::string colName = cit->first.as<std::string>().substr(1,cit->first.as<std::string>().length()-1);
				UVec4 colVal = {cit->second["r"].as<float>(),cit->second["g"].as<float>(),cit->second["b"].as<float>(),cit->second["a"].as<float>()};
				mat->m_Colors[colName] = colVal;
			}
		}

		if(it->first.as<std::string>() == "m_Textures")
		{
			YAML::Node textures = it->second;
			for(YAML::const_iterator cit=textures.begin(); cit!=textures.end(); cit++) 
			{
				std::string texName = cit->first.as<std::string>().substr(1,cit->first.as<std::string>().length()-1);
				std::string filePath = "../assets" + cit->second["m_TextureFile"].as<std::string>();
				Texture* tex = ResourceManager::LoadTexture(filePath.c_str());
				tex->name = texName;
				mat->m_Textures[texName] = tex;
			}
		}

	}

	if(materialData["m_ShaderFile"] != nullptr)
	{
		std::string vertSource = "../assets" + materialData["m_ShaderFile"].as<std::string>() + ".vs";
		std::string fragSource = "../assets" + materialData["m_ShaderFile"].as<std::string>() + ".fs";
		mat->m_Shader = ResourceManager::LoadShader(vertSource.c_str(), fragSource.c_str());
	}
	else
		mat->m_Shader = ResourceManager::defaultShader;

    mat->m_Name = materialData["m_Name"].as<std::string>();
    return mat;
}

Surface* processMesh(aiMesh * mesh, const aiScene * scene)
{
    Surface* surfPtr = new Surface();

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		surfPtr->vertices.push_back({mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z});
		surfPtr->normals.push_back({mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z});
		if(mesh->mColors[0] != nullptr)
			surfPtr->colors.push_back({mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b});
		else
			surfPtr->colors.push_back({1.0f,1.0f,1.0f});
		if(mesh->mTextureCoords[0])
		{
			surfPtr->uvs.push_back({mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
		}
		else
		{
			surfPtr->uvs.push_back({0.0f, 0.0f});
		}
		if(mesh->mTangents != nullptr)
			surfPtr->tangents.push_back({mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z});
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			surfPtr->indices.push_back(face.mIndices[j]);
	}

	// Process materials from loaded model - create basic material based on default one!
	// TODO: consider loading default materials settings, or substitue this with default material instance.
	
	// Think about a way to translate stadnard material into PBR default material?
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
	// 			ResourceManager::LoadTexture(texture_path.c_str(), texString.C_Str()
	// 		}
	// 		mat->specularTexture = ResourceManager::GetTexture(texString.C_Str());
	// 	}

	// 	materials.push_back(mat);
	// }
    return surfPtr;
}

void processNode(aiNode * node, const aiScene * scene, GLBObject* rootObject)
{
	// Node is an object on 3D scene
	GLBObject* nodeObj = new GLBObject();
	nodeObj->name = node->mName.C_Str();
    // process each mesh located at the current node
	if(node->mNumMeshes > 0)
	{
		MeshRendererComponent* mrc = new MeshRendererComponent();
		Mesh* meshResource = new Mesh();
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// Add all meshes to object.
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshResource->AddSurface(processMesh(mesh, scene));
		}
		mrc->m_Mesh = meshResource;
		mrc->m_Material = ResourceManager::defaultMaterial;
		nodeObj->AddComponent(mrc);

	}
	nodeObj->transform.parent = &rootObject->transform;
	rootObject->children.push_back(nodeObj);
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], scene, nodeObj);
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
	objectToReturn->name = scene->mName.C_Str();

	processNode(scene->mRootNode, scene, objectToReturn);

    return objectToReturn;
}

Texture *ResourceManager::LoadTexture(const char *textureFilePath)
{
    _UUID objectID = LoadMetadata(textureFilePath);
	Texture* textureObj = new Texture();

	int w,h,nc;
	unsigned char *data = stbi_load(textureFilePath, &w, &h, &nc, 0);

	glActiveTexture(GL_TEXTURE0);

	//Since this is only OpenGL I can do that
	glBindTexture(GL_TEXTURE_2D, textureObj->id);
	//Make sure we only load RGB or RGBA images!

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, nc == 3 ? GL_RGB : GL_RGBA, w, h, 0, nc == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	textureObj->width = w;
	textureObj->height = h;

	stbi_image_free(data);
	return textureObj;
}
