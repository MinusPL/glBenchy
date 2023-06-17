#include "arm_target_mover.h"
#include "../../global/glbtime.h"

void ArmTargetMover::Update()
{
    UVec3 nv = HMM_Norm(currentPoint->WorldPosition() - mp_Object->transform.WorldPosition());
    if(HMM_Len(mp_Object->transform.Position() - currentPoint->Position()) <= 1.05f)
    {
        currentPoint = dir ? &_point1->transform : &_point2->transform;
        dir = !dir;
    }
    mp_Object->transform.Position(mp_Object->transform.Position() + (HMM_Norm(currentPoint->WorldPosition() - mp_Object->transform.WorldPosition()) * moveSpeed * Time::deltaTime));

}
