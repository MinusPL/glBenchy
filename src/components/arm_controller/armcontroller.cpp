#include "armcontroller.h"
#include "../../global/glbtime.h"
#include "../../core/debug/debug.h"

#define SAMPLING_DISTANCE 15.0f
#define DISTANCE_THRESHOLD 0.01f



UVec3 ArmControllerComponent::ForwardKinematics(std::vector<float>& angles)
{
    UVec3 prevPoint = _joints[0]->mp_Object->transform.WorldPosition();
    UQuat rot = {0.0f,0.0f,0.0f,1.0f};
    for(int i = 1; i < _joints.size(); i++)
    {
        rot *= HMM_QFromAxisAngle_RH(_joints[i - 1]->rotationAxis, HMM_AngleDeg(angles[i - 1]));
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
    //TODO: Add return to initial pose;
    if (DistanceFromTarget(target, angles) < DISTANCE_THRESHOLD)
        return;
    for(int i = 0; i < _joints.size(); i++)
    {
        float g = PartialGradient(target, angles, i);
        angles[i] -= g * 5000.0f * (float)Time::deltaTime;
        //if(angles[i] > 360.f) angles[i] -= 360.f;

        if(_angleLimits[i].X != 0.0f && _angleLimits[i].Y != 0.0f)
            angles[i] = HMM_Clamp(_angleLimits[i].X, angles[i], _angleLimits[i].Y);

        if (DistanceFromTarget(target, angles) < DISTANCE_THRESHOLD)
            return;
        _joints[i]->mp_Object->transform.Rotation(HMM_QFromAxisAngle_RH(_joints[i]->rotationAxis, HMM_AngleDeg(angles[i])));
    }
}

void ArmControllerComponent::Update()
{
    if(_targetPtr != nullptr)
    {
        InverseKinematics(_targetPtr->transform.WorldPosition(), _angles);
        //Debug::DrawLine(mp_Object->transform.WorldPosition(), _targetPtr->transform.Position(), {1.0f,0.0f,0.0f,1.0f});
    }
}
