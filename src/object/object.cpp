#include "object.h"

#include <vector>

GLBObject::GLBObject() :
    id(UUIDGenerator::GetUUID()), disabled(false)
{
    
}

void GLBObject::Update()
{
    for(auto& component : components)
        component.Update();
}