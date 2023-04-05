#ifndef CAMERA_H
#define CAMERA_H

#include "../component.h"
#include "../../handmademath/math.h"

enum CameraType
{
    APPLICATION,
    EDITOR
};

class CameraComponent : public GLBComponent
{
public:
    bool otrhographic;
    UVec2 orthographicSize;
    float fov;
    float aspectRatio;
    float nearClipPlane;
    float farClipPlane;

    UMat4 projection;
    UMat4 view;

    CameraComponent();
    ~CameraComponent();

    void Update() override;

    void SetCameraMode(bool orthographic);

    static CameraComponent* main;
    
};

#endif