#ifndef LIGHT_H
#define LIGHT_H

#include "../component.h"
#include "../../core/shader/shader.h"

#define PIXEL_LIGHT_NUM 8

enum LightType
{
    DIRECTIONAL_LIGHT,
    POINT_LIGHT,
    SPOT_LIGHT
    //No area lights since these require alot of math iirc, and I am kinda lazy (～o￣3￣)～
};

struct Light
{
    LightType type;
    //This will be either 3D space position for point lights or direction for directional lights
    UVec4 position;
    UVec4 spotDirection;
    UVec4 color = {1.0f,1.0f,1.0f,1.0f};

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    float spotAngle;
    float softSpotAngle;
};

class LightComponent : public GLBComponent
{
    void Update() override;
public:
    LightComponent();
    Light lightData;
    static std::vector<Light> GetLights();
    //FIXME: Temporary solution, will be removed later!
    static std::vector<Light*> lights;
};

#endif