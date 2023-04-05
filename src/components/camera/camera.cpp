#include "camera.h"
#include "../../core/scene_object/scene_object.h"


CameraComponent* CameraComponent::main = nullptr;

void CameraComponent::Update()
{git
    view = LookAt(this->mp_Object->transform.position, this->mp_Object->transform.Position+this->mp_Object->transform.Forward)
}

void CameraComponent::SetCameraMode(bool orthographic)
{
    if(orthographic)
        projection = Orthographic(-orthographicSize.X, orthographicSize.X, -orthographicSize.Y, orthographicSize.Y, nearClipPlane, farClipPlane);
    else
        projection = Perspective(fov, aspectRatio, nearClipPlane, farClipPlane);
}
