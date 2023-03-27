#ifndef MATERIAL_H
#define MATERIAL_H

#include "../shader/shader.h"
#include "../../global/uuid.h"

#include <unordered_map>
#include <string>

#include "../object/object.h"

//TODO this thing will store all properties from shader linked to this material.
class Material : public Object
{
public:
    std::string m_Name;
    Shader* m_Shader;
    std::unordered_map<std::string, int> m_Ints;
    std::unordered_map<std::string, int> m_Floats;
    std::unordered_map<std::string, int> m_Textures;
    std::unordered_map<std::string, int> m_Colors;

    Material();

    //I don't think more stuff needs to be saved in materials, since we're going to pass Ints, Floats, Textures and Colors only.
};

#endif