#include "stdafx.h"

void DisableController(Uint8 index) {
    if (index && index != 1)
    {
        ControllerEnabled[0] = 0;
        ControllerEnabled[1] = 0;
    }
    else {
        ControllerEnabled[index] = 0;
    }
}


float __stdcall squareroot(float a1)
{
    double result; // st7

    if (a1 >= 0.0)
    {
        result = sqrt(a1);
    }
    else
    {
        result = -sqrt(a1 * -1.0);
    }
    return result;
}


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

signed int TailsAI_RangeOut(EntityData1* p1, EntityData1* p2, ObjectMaster* a3)
{
    const NJS_VECTOR* p2Pos; // esi
    double v4; // st7
    double v5; // st6
    double v6; // st5
    float v7; // ST00_4
    double v8; // st7
    signed int result; // eax
    EntityData1* v10; // eax
    Sint16 v11; // cx
    Vector3 a3a; // [esp+8h] [ebp-Ch]

    p2Pos = &p2->Position;
    v4 = p2->Position.x - p1->Position.x;
    v5 = p2->Position.y - p1->Position.y;
    v6 = p2->Position.z - p1->Position.z;
    v7 = v6 * v6 + v5 * v5 + v4 * v4;
    v8 = squareroot(v7);
    if (v8 > 1000.0)
    {

    }
    if (v8 >= 50.0)
    {

        result = 0;
    }

    return result;
}