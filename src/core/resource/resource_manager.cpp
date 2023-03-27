#include "resource_manager.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include <yaml-cpp/yaml.h>

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
