#include "stdafx.h"


void GetPlayerSidePos(NJS_VECTOR* v, EntityData1* e, float m)
{
    Float _sin; // ST00_4

    if (e)
    {
        if (v)
        {
            _sin = sin(e->Rotation.y);
            v->x = e->Position.x - cos(e->Rotation.y) * m;
            v->y = e->Position.y;
            v->z = e->Position.z - _sin * m;
        }
    }
}


void __cdecl SetToCameraPosition(NJS_VECTOR* a1)
{
    NJS_VECTOR* v1; // eax

    v1 = &CameraData.Position;
    a1->x = CameraData.Position.x;
    a1->y = v1->y;
    a1->z = v1->z;
}