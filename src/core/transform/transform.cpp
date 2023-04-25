#include "transform.h"

Transform::Transform() :
    position(Vec3(0.f,0.f,0.f)), rotation(Quaternion(0.f,0.f,0.f,1.f)), scale(Vec3(1.f,1.f,1.f))
{
    UpdateVectors();
}

Transform::~Transform()
{
    parent = nullptr;
}

void Transform::UpdateVectors()
{
	UMat4 rotMat = QuaternionToMat4(rotation);
    forward = NormalizeVec3((rotMat * Vec4(0,0,1,1)).XYZ);
    right = NormalizeVec3((rotMat * Vec4(1,0,0,1)).XYZ);
    up = NormalizeVec3((rotMat * Vec4(0,1,0,1)).XYZ);
}

UMat4 Transform::ApplyTransform()
{
    UMat4 tlm  = Translate(position);
    UMat4 rlm  = QuaternionToMat4(rotation);
    UMat4 slm  = Scale(scale);

    if(parent != nullptr)
        return parent->ApplyTransform()*tlm*rlm*slm;
    else
        return tlm*rlm*slm;
}