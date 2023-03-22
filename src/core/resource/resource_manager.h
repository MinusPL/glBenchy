#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "../mesh/mesh.h"
#include "../shader/shader.h"

class ResourceManager
{
public:
    static Shader LoadShader(const char* vertexPath, const char* fragmentPath);
};

#endif