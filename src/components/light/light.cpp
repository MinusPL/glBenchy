#include "light.h"

#include "../../core/scene_object/scene_object.h"

std::vector<Light*> LightComponent::lights;

void LightComponent::Update()
{
    switch(lightData.type)
    {
        case DIRECTIONAL_LIGHT:
            lightData.position = HMM_V4(this->mp_Object->transform.Forward().X,
                                        this->mp_Object->transform.Forward().Y,
                                        this->mp_Object->transform.Forward().Z, 1.0f);
            break;
        case POINT_LIGHT:
            lightData.position = HMM_V4(this->mp_Object->transform.Position().X,
                                        this->mp_Object->transform.Position().Y,
                                        this->mp_Object->transform.Position().Z, 0.0f);
            lightData.spotDirection = {0.0f,0.0f,1.0f,0.0f};
            break;
        case SPOT_LIGHT:
            lightData.position = HMM_V4(this->mp_Object->transform.Position().X,
                                this->mp_Object->transform.Position().Y,
                                this->mp_Object->transform.Position().Z, 0.0f);
            lightData.spotDirection = HMM_V4(this->mp_Object->transform.Forward().X,
                                        this->mp_Object->transform.Forward().Y,
                                        this->mp_Object->transform.Forward().Z, 0.0f);
            break;
    }
}

LightComponent::LightComponent() : GLBComponent(), lightData(Light{})
{
    lights.push_back(&lightData);
}

//TODO: Think about making it choose lights based on current brightness. Maybe based on attenuation value to camera position.
std::vector<Light> LightComponent::GetLights()
{
    std::vector<Light> lightsToReturn;



    return lightsToReturn;
}
