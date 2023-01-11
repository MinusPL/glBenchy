#include "object.h"

#include <vector>

GLBObject::GLBObject() :
    id(UUIDGenerator::GetUUID()), disabled(false)
{
}

void GLBObject::Update()
{
    printf("Update called from object[%s]\n", UUIDGenerator::UUIDToString(id).c_str());
    for(auto& component : components)
        component.Update();
}