#include "armcontroller.h"
#include "../../core/debug/debug.h"

#define SAMPLING_DISTANCE 5.0f

UVec3 ArmControllerComponent::ForwardKinematics(std::vector<float>& angles)
{
    UVec3 prevPoint = _joints[0]->mp_Object->transform.Position();
    UQuat rot = {0.0f,0.0f,0.0f,1.0f};
    for(int i = 1; i < _joints.size(); i++)
    {
        rot = rot * HMM_QFromAxisAngle_LH(_joints[i - 1]->rotationAxis, HMM_AngleDeg(angles[i - 1]));
        //UVec4 tmp = HMM_QToM4(rot) * HMM_V4(_joints[i]->startingOffset.X, _joints[i]->startingOffset.Y, _joints[i]->startingOffset.Z, 1.0f);
        UVec3 nextPoint = prevPoint + QMulV3(rot, _joints[i]->startingOffset);
        prevPoint = nextPoint;
    }
    return prevPoint;
}

float ArmControllerComponent::DistanceFromTarget(UVec3 target, std::vector<float>& angles)
{
    UVec3 point = ForwardKinematics(angles);
    Debug::DrawLine(mp_Object->transform.Position(), point, {0.0f,0.0f,1.0f,1.0f});
    return HMM_LenV3(point-target);
}

float ArmControllerComponent::PartialGradient(UVec3 target, std::vector<float> &angles, int i)
{
    float angle = angles[i];

    float f_x = DistanceFromTarget(target, angles);
    angles[i] += SAMPLING_DISTANCE;
    float f_x_plus_d = DistanceFromTarget(target, angles);
    float gradient = (f_x_plus_d - f_x) / SAMPLING_DISTANCE;
    // Restores
    angles[i] = angle;
    return gradient;
}

void ArmControllerComponent::InverseKinematics(UVec3 target, std::vector<float> &angles)
{
    for(int i = _joints.size()-1; i >= 0; i--)
    {
        float g = PartialGradient(target, angles, i);
        angles[i] -= 2.0f * g;
        _joints[i]->mp_Object->transform.Rotation(/*_joints[i]->mp_Object->transform.Rotation() **/QuatFromEuler(_joints[i]->rotationAxis * angles[i]));
    }
}

void ArmControllerComponent::Update()
{
    if(_targetPtr != nullptr)
    {
        InverseKinematics(_targetPtr->transform.Position(), _angles);
        Debug::DrawLine(mp_Object->transform.Position(), _targetPtr->transform.Position(), {1.0f,0.0f,0.0f,1.0f});
    }
}
