#include "scene_object.h"

#include <vector>

#include "../scene/scene_manager.h"
#include "../scene/scene.h"

GLBObject::GLBObject() :
    disabled(false), Object()
{
    
}

void GLBObject::Update()
{
    for(auto& component : components)
    {
        if(!component->hasStarted)
        {
            component->Start();
            component->hasStarted = true;
        }
        component->Update();
    }
}

//should it be here?
void GLBObject::Draw()
{
    for(auto& component : components)
        component->Draw();
}

void GLBObject::AddComponent(GLBComponent* componentPtr)
{
    componentPtr->mp_Object = this;
    components.push_back(componentPtr);
}

GLBObject* GLBObject::FindWithTag(std::string tag)
{
    for(auto& obj : SceneManager::activeScene->hierarchy)
    {
        auto pos = obj.second->tags.find(tag);
        if(pos != obj.second->tags.end())
            return obj.second;
    }
    return nullptr;
}