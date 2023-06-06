#ifndef ARMCONTROLLER_H
#define ARMCONTROLLER_H

#include "../component.h"
#include "../../core/scene_object/scene_object.h"
#include "armjoint.h"

class ArmControllerComponent : public GLBComponent
{   
public:
    std::vector<ArmJoint*> _joints;
    std::vector<float> _angles;
    std::vector<UVec2> _angleLimits;
    GLBObject* _targetPtr;
private:
    UVec3 ForwardKinematics(std::vector<float>& angles);
    float DistanceFromTarget(UVec3 target, std::vector<float>& angles);
    float PartialGradient(UVec3 target, std::vector<float>& angles, int i);
    void InverseKinematics(UVec3 target, std::vector<float>& angles);
public:
    void Update() override;
};

#endif