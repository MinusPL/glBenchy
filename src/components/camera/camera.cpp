#include "camera.h"
#include "../../core/scene_object/scene_object.h"


CameraComponent* CameraComponent::main = nullptr;
CameraComponent* CameraComponent::current = nullptr;

CameraComponent::CameraComponent()
{
    orthographic = false;
    fov = 45.0f;
    aspectRatio = 16.0f/9.0f;
    nearClipPlane = 0.1f;
    farClipPlane = 100.0f;
    projection = Perspective(fov, aspectRatio, nearClipPlane, farClipPlane);
    //set as main camera if there is no other camera in scene hierarchy.
    if(CameraComponent::main == nullptr) CameraComponent::main = this;
    CameraComponent::current = CameraComponent::main;
}

void CameraComponent::Update()
{
    view = LookAt(this->mp_Object->transform.Position(), this->mp_Object->transform.Position() + this->mp_Object->transform.Forward(), this->mp_Object->transform.Up());
}

void CameraComponent::SetCameraMode(bool orthographic)
{
    this->orthographic = orthographic;
    if(orthographic)
        projection = Orthographic(-orthographicSize.X, orthographicSize.X, -orthographicSize.Y, orthographicSize.Y, nearClipPlane, farClipPlane);
    else
        projection = Perspective(fov, aspectRatio, nearClipPlane, farClipPlane);
}
