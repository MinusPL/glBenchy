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

struct ModelMetadata
{
	std::map<std::string, std::string> materialOverrides;
};

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

	if(materialData["m_Transparent"] != nullptr)
		mat->m_Translucent = materialData["m_ShaderFile"].as<bool>();
	else
		mat->m_Translucent = false;

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
    return surfPtr;
}

void processNode(aiNode * node, const aiScene * scene, GLBObject** rootObject, ModelMetadata& modelData)
{
	if(node->mParent == nullptr && node->mNumChildren < 2 && std::string(node->mName.C_Str()).find("RootNode") != std::string::npos)
	{
		processNode(node->mChildren[0], scene, rootObject, modelData);
		return;
	}
	// Node is an object on 3D scene
	GLBObject* nodeObj = new GLBObject();
	nodeObj->name = node->mName.C_Str();
	aiVector3D nodePos, nodeScale;
	aiQuaternion nodeRot;
	node->mTransformation.Decompose(nodeScale, nodeRot, nodePos);
	//Apply Transformation
	nodeObj->transform.Position({nodePos.x, nodePos.y, nodePos.z});
	nodeObj->transform.Rotation({nodeRot.x, nodeRot.y, nodeRot.z, nodeRot.w});
	nodeObj->transform.Scale({nodeScale.x, nodeScale.y, nodeScale.z});
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
			//Add material per mesh!
			
			if(mesh->mMaterialIndex >= 0)
			{
				aiMaterial *importedMaterial = scene->mMaterials[mesh->mMaterialIndex];
				Material newMaterial = *ResourceManager::defaultMaterial;
				newMaterial.m_Name = importedMaterial->GetName().C_Str();
				aiColor3D diffColor;
				importedMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffColor);
				newMaterial.m_Colors["baseColor"] = {diffColor.r, diffColor.g, diffColor.b, 1.0f};

				if(modelData.materialOverrides.contains(importedMaterial->GetName().C_Str()))
				{
					if(modelData.materialOverrides.at(importedMaterial->GetName().C_Str()).length() > 0)
					{
						std::string matPath = "../assets" + modelData.materialOverrides.at(importedMaterial->GetName().C_Str());
						Material* matPtr = ResourceManager::LoadMaterial(matPath.c_str());
						newMaterial = *matPtr;
						delete matPtr;
					}
				}
				else
				{
					modelData.materialOverrides[importedMaterial->GetName().C_Str()] = "";

				}
				mrc->m_Materials.push_back(newMaterial);
			}
			else
				mrc->m_Materials.push_back(*ResourceManager::defaultMaterial);
		}
		mrc->m_Mesh = meshResource;
		nodeObj->AddComponent(mrc);
	}

	if(*rootObject != nullptr)
	{
		nodeObj->transform.parent = &((*rootObject)->transform);
		(*rootObject)->children.push_back(nodeObj);
	}
	else *rootObject = nodeObj;
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], scene, &nodeObj, modelData);
}

//TODO: Fix two additional nodes when loading model.
GLBObject* ResourceManager::LoadModel(const char *modelFilePath)
{
    bool shouldSaveMetadata = true;
	ModelMetadata mData;
	std::filesystem::path p(std::string(modelFilePath)+".meta");
    if(std::filesystem::exists(p))
    {
		shouldSaveMetadata = false;
		YAML::Node fileData = YAML::LoadFile(p.string());
		YAML::Node materialData = fileData["Materials"];
		for(YAML::const_iterator it=materialData.begin(); it!=materialData.end(); it++) 
		{
			mData.materialOverrides[it->first.as<std::string>()] = it->second.as<std::string>();
		}
	}
    //Time for Assimp processing!
    Assimp::Importer import;
	const aiScene *scene = import.ReadFile(modelFilePath, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return nullptr;
	}

	GLBObject* objectToReturn = nullptr;

	processNode(scene->mRootNode, scene, &objectToReturn, mData);

	if(shouldSaveMetadata)
	{
        _UUID uuidData = UUIDGenerator::GetUUID();
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "guid";
        out << YAML::Value << UUIDGenerator::UUIDToString(uuidData);
		out << YAML::Key << "Materials";
		out << YAML::Value << YAML::BeginMap;
		for(auto matOverride : mData.materialOverrides)
		{
        	out << YAML::Key << matOverride.first;
			out << YAML::Value << matOverride.second;
		}
		out<< YAML::EndMap;
        out << YAML::EndMap;
        std::ofstream outMetaFile(p);
        outMetaFile << out.c_str();
        outMetaFile.close();
	}

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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, nc == 4 ? GL_CLAMP_TO_EDGE : GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, nc ==  4 ? GL_CLAMP_TO_EDGE : GL_REPEAT);
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
