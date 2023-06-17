#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../../handmademath/math.h"
#include <stdexcept>

//Since in C++ we do not have properties like in C# we need to make sure people cannot directly access these values
//Otherwise it will be hard to keep everything updated.
class Transform
{
    //Components of transform matrix

    UVec3 position;
    UQuat rotation;
    UVec3 scale;

    //Directionals

    UVec3 up;
    UVec3 right;
    UVec3 forward;

    //special hidden function to update all other aspects of transform class after we modify any part of it.
    void UpdateVectors();
public:

    Transform* parent = nullptr;

    //Getters for directional vectors in model space.
    //People shouldn't change these at all manually

    inline UVec3 Up() { return up; }
    inline UVec3 Right() { return right; }
    inline UVec3 Forward() { return forward; }


    inline UVec3 Position() { return position; }

    void Scale(UVec3 newScale)
    {
        scale = newScale;
        UpdateVectors();
    }

    void Position(UVec3 newPos)
    {
        position = newPos;
        UpdateVectors();
    }

    void Position(float x, float y, float z)
    {
		position = HMM_V3(x, y, z);
		UpdateVectors();
	}

    UVec3 WorldPosition()
    {
        if(parent != nullptr)
        {
            return position + parent->WorldPosition();
        }
        else
            return position;
    }

    inline UQuat Rotation() { return rotation; }
    inline UVec3 RotationEulerAngles() { return QuatToEuler(rotation); }

    void Rotation(UQuat newRotation)
    {
        rotation = newRotation;
        UpdateVectors();
    }

    void Rotation(float x, float y, float z)
    {
        rotation = QuatFromEuler(HMM_AngleDeg(x),HMM_AngleDeg(y),HMM_AngleDeg(z));
        UpdateVectors();
    }

    void Rotation(UVec3 rotAngles)
    {
        rotation = QuatFromEuler(HMM_AngleDeg(rotAngles.X), HMM_AngleDeg(rotAngles.Y), HMM_AngleDeg(rotAngles.Z));
        UpdateVectors();
    }

    void Rotate(float x, float y, float z, bool localSpace = true)
    {
        UQuat eulerQuat = QuatFromEuler(HMM_AngleDeg(x),HMM_AngleDeg(y),HMM_AngleDeg(z));
        if(localSpace)
            rotation = rotation * eulerQuat;
        else
            rotation = rotation * (HMM_InvQ(rotation) * eulerQuat * rotation);
        UpdateVectors();
    }

    void LookAt(UVec3 target, UVec3 worldUp = {0.0f,0.0f,1.0f})
    {
        rotation = HMM_M4ToQ_RH(HMM_LookAt_RH(position, target, worldUp));
        UpdateVectors();
    }

    UMat4 ApplyTransform();

    Transform();
    ~Transform();
};

#endif