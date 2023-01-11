#include "scene.h"

void GLBScene::Update()
{
    for(auto& obj : hierarchy)
        obj.second->Update();
}