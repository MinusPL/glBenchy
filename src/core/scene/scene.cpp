#include "scene.h"

void Scene::Update()
{
    for(auto& obj : hierarchy)
        obj.second->Update();
}

void Scene::Draw()
{
    for(auto& obj : hierarchy)
        obj.second->Draw();
}