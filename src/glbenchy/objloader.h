#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "../core/scene_object/scene_object.h"

class ObjLoader
{
public:
    static GLBObject* CreateArm();
    static GLBObject* LoadHall();
    static GLBObject* CreateWholeScreenQuad();
};

#endif