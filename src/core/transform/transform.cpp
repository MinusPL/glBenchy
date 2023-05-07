#include "transform.h"

Transform::Transform() :
    position(HMM_V3(0.f,0.f,0.f)), rotation(HMM_Q(0.f,0.f,0.f,1.f)), scale(HMM_V3(1.f,1.f,1.f))
{
    UpdateVectors();
}

Transform::~Transform()
{
    parent = nullptr;
}

void Transform::UpdateVectors()
{
	UMat4 rotMat = HMM_QToM4(rotation);
    forward = HMM_Norm((rotMat * HMM_V4(0,0,1,1)).XYZ);
    right = HMM_Norm((rotMat * HMM_V4(1,0,0,1)).XYZ);
    up = HMM_Norm((rotMat * HMM_V4(0,1,0,1)).XYZ);
}

UMat4 Transform::ApplyTransform()
{
    UMat4 tlm  = HMM_Translate(position);
    UMat4 rlm  = HMM_QToM4(rotation);
    UMat4 slm  = HMM_Scale(scale);

    if(parent != nullptr)
        return parent->ApplyTransform()*tlm*rlm*slm;
    else
        return tlm*rlm*slm;
}