#ifndef ARMJOINT_H
#define ARMJOINT_H

#include "../component.h"
#include "../../handmademath/math.h"

class ArmJoint : public GLBComponent
{
public:
    UVec3 rotationAxis;
    UVec3 startingOffset;
    void Start() override;
};

#endif