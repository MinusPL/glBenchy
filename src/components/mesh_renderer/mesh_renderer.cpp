#include "mesh_renderer.h"
#include "../../core/scene_object/scene_object.h"
#include "../camera/camera.h"

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
            m_Material->m_Shader->SetVector3f("mainLightDir", {1.2f, 8.5f, 10.0f});
            //Draw GL object
            glBindVertexArray(surf->VAO);
            glDrawElements(surf->vertexFlag, surf->indices.size(), GL_UNSIGNED_INT, (void*)0);
            glBindVertexArray(0);
        }
    }
}
