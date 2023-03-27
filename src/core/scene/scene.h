#ifndef SCENE_H
#define SCENE_H

#include <cstdint>
#include <map>

#include "../scene_object/scene_object.h"

/**
 * Class representing scene inside GLBenchy application.
 * This scene contains hierarchy of objects and provides world coordinate system.
*/
class Scene
{
public:
    std::string sceneName;
    std::map<uint64_t, GLBObject*> hierarchy;
    void Update();
    void Draw();
};

#endif