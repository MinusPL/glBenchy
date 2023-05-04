#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <unordered_map>

#include "../mesh/mesh.h"
#include "../shader/shader.h"
#include "../material/material.h"
#include "../object/object.h"
#include "../../global/uuid.h"
#include "../scene_object/scene_object.h"

class ResourceManager
{
public:
    static std::unordered_map<_UUID, Object*> resources;
    static Shader* LoadShader(const char* vertexPath, const char* fragmentPath);
    static Material* LoadMaterial(const char* materialFilePath);
    static GLBObject* LoadModel(const char* modelFilePath);
    static Shader* defaultShader;
    static Material* defaultMaterial;
    template <typename T>
    static inline T* GetResource(_UUID id)
    {
        return (T*)ResourceManager::resources[id];
    }
    static inline Object* GetResource(_UUID id)
    {
        return ResourceManager::resources[id];
    }
};

#endif


