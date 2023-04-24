#include "mesh_renderer.h"
#include "../../core/scene_object/scene_object.h"
#include "../camera/camera.h"

void MeshRendererComponent::Draw()
{
    if(CameraComponent::current != nullptr)
    {
        UMat4 model = this->mp_Object->transform.ApplyTransform();
        CameraComponent::current->view;
        UMat4 mvp = CameraComponent::current->projection * CameraComponent::current->view * model;
        m_Material.m_Shader->SetMatrix4("MVP", mvp);
        m_Mesh.Draw();
    }
}
