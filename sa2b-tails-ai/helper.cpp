#include "stdafx.h"
#include <math.h>
#include "SA2ModLoader.h"
#include "helper.h"

NJS_VECTOR PositionBuffer[MaxPlayers * 256];
Rotation RotationBuffer[MaxPlayers * 256];
int PosRotBufferIndex[MaxPlayers];

void PadReadOffP_(int index)
{
    if (index && index != 1)
    {
        ControllerEnabled[0] = false;
        ControllerEnabled[1] = false;
    }
    else
    {
        ControllerEnabled[index] = false;
    }
}

void PadReadOnP_(int index)
{
    if (index && index != 1)
    {
        ControllerEnabled[0] = true;
        ControllerEnabled[1] = true;
    }
    else
    {
        ControllerEnabled[index] = true;
    }
}

float njSqrt_(float f)
{
    if (f >= 0.0f)
    {
        return sqrtf(f);
    }
    else
    {
        return -sqrtf(f * -1.0f);
    }
}

float ghUnitVector_(NJS_VECTOR* vo, NJS_VECTOR* vd)
{
    float nmag = 1.0f / njSqrt_(vo->z * vo->z + vo->y * vo->y + vo->x * vo->x);
    vd->x = vo->x * nmag;
    vd->y = vo->y * nmag;
    vd->z = vo->z * nmag;
    return nmag;
}

float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest)
{
    return njSqrt_((dest->x - orig->x) * (dest->x - orig->x)
        + (dest->y - orig->y) * (dest->y - orig->y)
        + (dest->z - orig->z) * (dest->z - orig->z));
}

void GetPlayerSidePos(NJS_VECTOR* pos, EntityData1* twp, float dist)
{
    if (twp)
    {
        if (pos)
        {
            pos->x = twp->Position.x - njCos(twp->Rotation.y) * dist;
            pos->y = twp->Position.y;
            pos->z = twp->Position.z - njSin(twp->Rotation.y) * dist;
        }
    }
}

bool SetPlayerPosition_(int playerNum, int unk, NJS_VECTOR* pos, Rotation* rot) {
    int index = PosRotBufferIndex[playerNum] - unk;

    if (!MainCharacter[playerNum]) {
        return false;
    }

    if (pos) {
        PositionBuffer[playerNum + (MaxPlayers * index)] = *pos;
    }

    if (rot) {
        RotationBuffer[playerNum + (MaxPlayers * index)].x = rot->x;
        RotationBuffer[playerNum + (MaxPlayers * index)].y = rot->y;
        RotationBuffer[playerNum + (MaxPlayers * index)].z = rot->z;
    }

    return true;
}

void SetToCameraPosition(NJS_VECTOR* pos, int pnum)
{
    *pos = CameraScreensInfoArray[pnum]->pos;
}

void SetInputP_(EntityData1* data, GeneralActions gaction)
{
    data->NextAction = gaction;
    data->Status |= Status_DoNextAction;
}