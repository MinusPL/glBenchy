#include "armcontroller.h"

#define SAMPLING_DISTANCE 5.0f

UVec3 ArmControllerComponent::ForwardKinematics(std::vector<float>& angles)
{
    UVec3 prevPoint = _joints[0]->mp_Object->transform.Position();
    UQuat rot = {0.0f,0.0f,0.0f,1.0f};
    for(int i = 1; i < _joints.size(); i++)
    {
        rot = rot * HMM_QFromAxisAngle_LH(_joints[i - 1]->rotationAxis, angles[i - 1]);
        //UVec4 tmp = HMM_QToM4(rot) * HMM_V4(_joints[i]->startingOffset.X, _joints[i]->startingOffset.Y, _joints[i]->startingOffset.Z, 1.0f);
        UVec3 nextPoint = prevPoint + QMulV3(rot, _joints[i]->startingOffset);
        prevPoint = nextPoint;
    }
    return prevPoint;
}

float ArmControllerComponent::DistanceFromTarget(UVec3 target, std::vector<float>& angles)
{
    UVec3 point = ForwardKinematics(angles);
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
    for(int i = 0; i < _joints.size(); i++)
    {
        float g = PartialGradient(target, angles, i);
        angles[i] -= 2.0f * g;
        UVec3 prevRot = _joints[i]->mp_Object->transform.RotationEulerAngles();
        float X = _joints[i]->rotationAxis.X == 1.0f ? angles[i] : prevRot.X;
        float Y = _joints[i]->rotationAxis.Y == 1.0f ? angles[i] : prevRot.Y;
        float Z = _joints[i]->rotationAxis.Z == 1.0f ? angles[i] : prevRot.Z;
        _joints[i]->mp_Object->transform.Rotation(X, Y, Z);
    }
}

void ArmControllerComponent::Update()
{
    if(_targetPtr != nullptr)
        InverseKinematics(_targetPtr->transform.Position(), _angles);
}
