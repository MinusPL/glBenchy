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

void Scene::AddChildrenOfObject(GLBObject* obj)
{
    for(auto cobj : obj->children)
    {
        hierarchy[objectCount++] = cobj;
        if(cobj->children.size() > 0)
            AddChildrenOfObject(cobj);
    }
}

void Scene::AddObject(GLBObject* obj)
{
    hierarchy[objectCount++] = obj;
    if(obj->children.size() > 0)
        AddChildrenOfObject(obj);
}
