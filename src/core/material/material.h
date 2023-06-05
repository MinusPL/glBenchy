#ifndef MATERIAL_H
#define MATERIAL_H

#include "../shader/shader.h"
#include "../../global/uuid.h"

#include <unordered_map>
#include <string>

#include "../object/object.h"
#include "../../handmademath/math.h"

class Material : public Object
{
public:
    std::string m_Name;
    Shader* m_Shader;
    bool m_Translucent;
    UVec2 m_TextureScale;
    std::unordered_map<std::string, int> m_Ints;
    std::unordered_map<std::string, float> m_Floats;
    std::unordered_map<std::string, Texture*> m_Textures;
    std::unordered_map<std::string, UVec4> m_Colors;

    Material();

    void Use();
};

#endif