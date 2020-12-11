#include "stdafx.h"

NJS_VECTOR PositionBuffer[2048];
Rotation RotationBuffer[2048];
char PosRotBufferIndex[8];

void DisableController(int index) {
    if (index && index != 1) {
        ControllerEnabled[0] = false;
        ControllerEnabled[1] = false;
    }
    else {
        ControllerEnabled[index] = false;
    }
}

void EnableController(int index) {
    if (index && index != 1) {
        ControllerEnabled[0] = true;
        ControllerEnabled[1] = true;
    }
    else {
        ControllerEnabled[index] = true;
    }
}

float squareroot(float f) {
    if (f >= 0.0f) {
        return sqrtf(f);
    }
    else {
        return -sqrtf(f * -1.0);
    }
}

void GetPlayerSidePos(NJS_VECTOR* pos, EntityData1* entity, float dist) {
    if (entity) {
        if (pos) {
            pos->x = entity->Position.x - cos(entity->Rotation.y) * dist;
            pos->y = entity->Position.y;
            pos->z = entity->Position.z - sin(entity->Rotation.y) * dist;
        }
    }
}

signed int __cdecl SavePlayerPosition(unsigned __int8 playerNum, char a2, NJS_VECTOR* a3, NJS_VECTOR* a4)
{
    unsigned __int8 v4; // dl
    int i; // eax

    v4 = PosRotBufferIndex[playerNum] - a2;
    if (!MainCharacter[playerNum])
    {
        return 0;
    }
    if (a3)
    {
        PositionBuffer[playerNum + 8 * v4] = *a3;
    }
    if (a4)
    {
        i = playerNum + 8 * v4;
        RotationBuffer[i].x = a4->x;
        RotationBuffer[i].y = a4->y;
        RotationBuffer[i].z = a4->z;
    }
    return 1;
}

void SetToCameraPosition(NJS_VECTOR* pos) {
    pos->x = CameraData.Position.x;
    pos->y = CameraData.Position.y;
    pos->z = CameraData.Position.z;
}

void ForcePlayerAction(EntityData1* data, GeneralActions gaction) {
    data->NextAction = gaction;
    data->Status |= Status_DoNextAction;
}

float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest) {
    return squareroot(powf(dest->x - orig->x, 2) + powf(dest->y - orig->y, 2) + powf(dest->z - orig->z, 2));
}