#include "scene_object.h"

#include <vector>

GLBObject::GLBObject() :
    disabled(false), Object()
{
    
}

void GLBObject::Update()
{
    for(auto& component : components)
        component->Update();
}

//should it be here?
void GLBObject::Draw()
{
    for(auto& component : components)
        component->Draw();
}