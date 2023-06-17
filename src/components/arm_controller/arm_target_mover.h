#ifndef ARMTARGETMOVER_H
#define ARMTARGETMOVER_H

#include "../component.h"
#include "../../core/scene_object/scene_object.h"

class ArmTargetMover : public GLBComponent
{   
public:
    GLBObject* _point1;
    GLBObject* _point2;

    Transform* currentPoint;
    bool dir = false;

    float moveSpeed = 5.0f;

    void Update() override;
};

#endif