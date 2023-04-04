#include "camera.h"

CameraComponent* CameraComponent::main = nullptr;

void CameraComponent::SetCameraMode(bool orthographic)
{
    if(orthographic)
        projection = Orthographic(-orthographicSize.X, orthographicSize.X, -orthographicSize.Y, orthographicSize.Y, nearClipPlane, farClipPlane);
    else
        projection = Perspective(fov, aspectRatio, nearClipPlane, farClipPlane);
}
