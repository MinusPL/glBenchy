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
        //Use material and apply material / shader specific things.
        m_Material->Use();
        //apply all builtint variables for shader program to use. This block will be quite big!
        //Model, view, projection
        m_Material->m_Shader->SetMatrix4("GLB_MVP", mvp);
        m_Material->m_Shader->SetMatrix4("GLB_M", model);
        m_Material->m_Shader->SetMatrix4("GLB_ITM", HMM_Transpose(HMM_InvGeneralM4(model)));
        m_Material->m_Shader->SetMatrix4("GLB_V", CameraComponent::current->view);
        m_Material->m_Shader->SetMatrix4("GLB_P", CameraComponent::current->projection);
        m_Material->m_Shader->SetMatrix4("GLB_VP", CameraComponent::current->projection * CameraComponent::current->view);
        //Camera stuff
        m_Material->m_Shader->SetVector3f("WorldSpaceCameraPos", CameraComponent::current->mp_Object->transform.Position());
        //Lightning stuff

        //Todo move to light manager or something
        m_Material->m_Shader->SetVector3f("mainLightDir", {1.2f, 8.5f, 10.0f});
        m_Mesh->Draw();
    }
}
