#include "mesh_renderer.h"
#include "../../core/scene_object/scene_object.h"
#include "../camera/camera.h"

void MeshRendererComponent::Draw()
{
    if(CameraComponent::main != nullptr)
    {
        UMat4 model = this->mp_Object->transform.ApplyTransform();
        m_Mesh.Draw();
    }
}
