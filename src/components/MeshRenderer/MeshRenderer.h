#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include "../component.h"
#include "../../core/mesh/mesh.h"


class MeshRendererComponent : public GLBComponent
{
public:
    Mesh m_Mesh;
    void Draw();
};

#endif