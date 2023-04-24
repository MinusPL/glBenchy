#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "scene.h"
#include <unordered_map>
#include <string>

class SceneManager
{
public:
    static std::unordered_map<std::string, Scene*> scenes;
    static Scene* activeScene;
    static void SwitchScene(std::string name);

};

#endif