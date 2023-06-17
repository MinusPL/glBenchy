#include "mesh_renderer.h"
#include "../../core/scene_object/scene_object.h"
#include "../camera/camera.h"
#include "../light/light.h"

#include "../../global/glbtime.h"

void MeshRendererComponent::Draw()
{
    if(CameraComponent::current != nullptr)
    {
        UMat4 model = this->mp_Object->transform.ApplyTransform();
        //UMat4 mvp = model;
        UMat4 mvp = CameraComponent::current->projection * CameraComponent::current->view * model;
        
        //Do for each subsurface!
        for(int i = 0; i < m_Mesh->m_Surfaces.size(); i++)
        {
            //Get material and surface
            Material* m_Material = &m_Materials[i];
            Surface* surf = m_Mesh->m_Surfaces[i];
            //Use material
            m_Material->Use();
            //Apply built-in variables for shaders
            //Model, View, Projection
            m_Material->m_Shader->SetMatrix4("GLB_MVP", mvp);
            m_Material->m_Shader->SetMatrix4("GLB_M", model);
            m_Material->m_Shader->SetMatrix4("GLB_ITM", HMM_Transpose(HMM_InvGeneralM4(model)));
            m_Material->m_Shader->SetMatrix4("GLB_V", CameraComponent::current->view);
            m_Material->m_Shader->SetMatrix4("GLB_P", CameraComponent::current->projection);
            m_Material->m_Shader->SetMatrix4("GLB_VP", CameraComponent::current->projection * CameraComponent::current->view);
            //Camera
            m_Material->m_Shader->SetVector3f("WorldSpaceCameraPos", CameraComponent::current->mp_Object->transform.Position());
            //Lightning

            m_Material->m_Shader->SetFloat("Shininess", 256.0f);
            m_Material->m_Shader->SetInteger("UsedLightCount", (int)LightComponent::lights.size());

            m_Material->m_Shader->SetFloat("_SinTime", HMM_SINF((float)Time::time));
            m_Material->m_Shader->SetFloat("_Time", (float)Time::time);
            m_Material->m_Shader->SetVector2f("textureScaling", m_Material->m_TextureScale);

            for(int i = 0; i < LightComponent::lights.size(); i++)
            {
                char varName[512];
                sprintf(varName, "lights[%d]", i);
                std::string posStr = std::string(varName) + ".position";
                std::string spotDirStr = std::string(varName) + ".spotDirection";
                std::string constantStr = std::string(varName) + ".constant";
                std::string linearStr = std::string(varName) + ".linear";
                std::string quadraticStr = std::string(varName) + ".quadratic";
                std::string colorStr = std::string(varName) + ".color";
                std::string spotAngleStr = std::string(varName) + ".spotAngle";
                std::string softSpotAngleStr = std::string(varName) + ".softSpotAngle";


                m_Material->m_Shader->SetVector4f(posStr.c_str(), LightComponent::lights[i]->position);
                m_Material->m_Shader->SetFloat(constantStr.c_str(), LightComponent::lights[i]->constant); 
                m_Material->m_Shader->SetFloat(linearStr.c_str(), LightComponent::lights[i]->linear); 
                m_Material->m_Shader->SetFloat(quadraticStr.c_str(), LightComponent::lights[i]->quadratic); 
                m_Material->m_Shader->SetVector4f(colorStr.c_str(), LightComponent::lights[i]->color);
                if(LightComponent::lights[i]->type == SPOT_LIGHT)
                {
                    m_Material->m_Shader->SetVector4f(spotDirStr.c_str(), LightComponent::lights[i]->spotDirection);
                    m_Material->m_Shader->SetFloat(spotAngleStr.c_str(), cos(HMM_AngleDeg(LightComponent::lights[i]->spotAngle)));
                    m_Material->m_Shader->SetFloat(softSpotAngleStr.c_str(), cos(HMM_AngleDeg(LightComponent::lights[i]->softSpotAngle)));
                }
                else
                {
                    m_Material->m_Shader->SetVector4f(spotDirStr.c_str(), HMM_V4(0.0f,0.0f,1.0f,0.0f));
                    m_Material->m_Shader->SetFloat(spotAngleStr.c_str(), 1.0f);
                    m_Material->m_Shader->SetFloat(softSpotAngleStr.c_str(), 1.0f);
                }
            }
            //Draw GL object
            glBindVertexArray(surf->VAO);
            glDrawElements(surf->vertexFlag, (int)surf->indices.size(), GL_UNSIGNED_INT, (void*)0);
            glBindVertexArray(0);
        }
    }
}
