#pragma once

static constexpr int MaxPlayers = 2;

struct motionwk {
	CharObj2Base* work;
	NJS_POINT3 spd;
	NJS_POINT3 acc;
	Rotation ang_aim;
	Rotation ang_spd;
	float rad;
	float height;
	float weight;
};

DataArray(motionwk*, MainCharMotions, 0x1DE95E0, MaxPlayers);

enum GeneralActions {
	GeneralAction_Fly = 59
};

static Buttons JumpButtons = Buttons_A;

void ForcePlayerAction(EntityData1* data, GeneralActions gaction);
void DisableController(int index);
void EnableController(int index);
void GetPlayerSidePos(NJS_VECTOR* pos, EntityData1* entity, float dist);
void SetToCameraPosition(NJS_VECTOR* pos);
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest);