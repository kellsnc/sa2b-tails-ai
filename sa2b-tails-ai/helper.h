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
	GeneralAction_Fly = 37
};

static Buttons JumpButtons = Buttons_A;
static Buttons AttackButtons = Buttons_X;

void ForcePlayerAction(EntityData1* data, GeneralActions gaction);
void DisableController(int index);
void EnableController(int index);
void GetPlayerSidePos(NJS_VECTOR* pos, EntityData1* entity, float dist);
void SetToCameraPosition(NJS_VECTOR* pos);
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest);
signed int __cdecl SavePlayerPosition(unsigned __int8 playerNum, char a2, NJS_VECTOR* a3, NJS_VECTOR* a4);