#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include "../component.h"
#include "../../core/mesh/mesh.h"
#include "../../core/material/material.h"


class MeshRendererComponent : public GLBComponent
{
public:
    Mesh* m_Mesh;
    Material* m_Material;
    void Draw() override;
};

#endif