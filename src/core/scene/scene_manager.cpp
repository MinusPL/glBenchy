#include "scene_manager.h"

#include "../../components/camera/camera.h"
#include "../../components/component.h"

Scene* SceneManager::activeScene = nullptr;
std::unordered_map<std::string, Scene*> SceneManager::scenes;


void SceneManager::SwitchScene(std::string name)
{
    if(scenes.contains(name))
    {
        activeScene = scenes[name];
        GLBObject* cameraObject = GLBObject::FindWithTag("MainCamera");
        CameraComponent::main = cameraObject != nullptr ? (CameraComponent*)cameraObject->GetComponent<CameraComponent>() : nullptr;
        CameraComponent::current = CameraComponent::main;
    }
    else
        printf("Scene with name %s does not exist!\n", name.c_str());
}