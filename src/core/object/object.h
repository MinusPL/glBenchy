#ifndef OBJECT_H
#define OBJECT_H

#include <cstdint>
#include <vector>

#include "../../global/uuid.h"
#include "../../components/component.h"
#include "../transform/transform.h"

/**
 * Main class for objects used in GLB.
 * All objects have to derrive from this class to be properly initialized
*/
class GLBObject
{
public:
    _UUID id;
    GLBObject();
    std::vector<GLBComponent*> components;
    virtual void Update();
    virtual void Draw();
    Transform transform;
    bool disabled;
};

#endif