#include "resource_manager.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

static _UUID  LoadMetadata(const char* filePath)
{

}

Shader ResourceManager::LoadShader(const char *vertexPath, const char *fragmentPath)
{
    Shader sh;

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

    sh.Compile(vertexCode.c_str(), fragmentCode.c_str(), nullptr);

    return sh;
}

Material ResourceManager::LoadMaterial(const char *materialFilePath)
{
    return Material();
}
