#include "scene_manager.h"

Scene* SceneManager::activeScene = nullptr;
std::unordered_map<std::string, Scene*> SceneManager::scenes;