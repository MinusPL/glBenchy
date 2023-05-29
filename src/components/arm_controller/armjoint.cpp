#include "armjoint.h"

#include "../../core/scene_object/scene_object.h"

void ArmJoint::Start()
{
    startingOffset = mp_Object->transform.Position();
}