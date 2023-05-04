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
        m_Material->m_Shader->SetMatrix4("MVP", mvp);
        m_Material->m_Shader->SetMatrix4("model", model);
        //Todo move to light manager or something
        m_Material->m_Shader->SetVector3f("mainLightDir", {1.2f, 8.5f, 10.0f});
        m_Mesh->Draw();
    }
}
