#include "stdafx.h"

NJS_VECTOR PositionBuffer[MaxPlayers * 256];
Rotation RotationBuffer[MaxPlayers * 256];
int PosRotBufferIndex[MaxPlayers];

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

float normalizevector(NJS_VECTOR* vo, NJS_VECTOR* vd) {
    float nmag = 1.0f / squareroot(vo->z * vo->z + vo->y * vo->y + vo->x * vo->x);
    vd->x = vo->x * nmag;
    vd->y = vo->y * nmag;
    vd->z = vo->z * nmag;
    return nmag;
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

bool SavePlayerPosition(int playerNum, int unk, NJS_VECTOR* pos, Rotation* rot) {
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