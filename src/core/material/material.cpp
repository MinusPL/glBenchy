#include "material.h"

Material::Material() :
    m_Name(""), Object()
{

}

void Material::Use()
{
    m_Shader->Use();
    //Set uniform values from material in shader
    for(auto& i : m_Ints)
        m_Shader->SetInteger(i.first.c_str(), i.second);
    for(auto& i : m_Floats)
        m_Shader->SetFloat(i.first.c_str(), i.second);
    for(auto& i : m_Colors)
        m_Shader->SetVector4f(i.first.c_str(), i.second);
    //for(auto& i : m_Textures)
     //   m_Shader->SetInteger(i.first.c_str(), i.second);
}