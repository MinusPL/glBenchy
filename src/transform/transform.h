#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../handmademath/math.h"
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

    void Position(UVec3 newPos)
    {
        position = newPos;
        UpdateVectors();
    }

    void Position(float x, float y, float z)
    {
		position = Vec3(x, y, z);
		UpdateVectors();
	}

    inline UQuat Rotation() { return rotation; }

    void Rotation(UQuat newRotation)
    {
        rotation = newRotation;
        UpdateVectors();
    }

    void Rotation(float x, float y, float z)
    {
        throw std::runtime_error("Not implemented");
    }

    UMat4 ApplyTransform();

    Transform();
    ~Transform();
};

#endif