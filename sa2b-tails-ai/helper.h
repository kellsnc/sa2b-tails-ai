#pragma once

#define TARGET_DYNAMIC(name) ((decltype(name##_r)*)name##_t->Target())

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

ObjectFunc(Tails_AlphaDisplay, 0x74FF20);
DataArray(motionwk*, MainCharMotions, 0x1DE95E0, MaxPlayers);

enum GeneralActions {
	GeneralActions_12 = 12,
	GeneralActions_24 = 24,
	GeneralAction_Fly = 29
};

static Buttons JumpButtons = Buttons_A;
static Buttons AttackButtons = Buttons_X;

void ForcePlayerAction(EntityData1* data, GeneralActions gaction);
void DisableController(int index);
void EnableController(int index);
void GetPlayerSidePos(NJS_VECTOR* pos, EntityData1* entity, float dist);
void SetToCameraPosition(NJS_VECTOR* pos);
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest);
bool SavePlayerPosition(int playerNum, int unk, NJS_VECTOR* pos, Rotation* rot);
float normalizevector(NJS_VECTOR* vo, NJS_VECTOR* vd);

static const void* const CheckRangeOutPtr = (void*)0x488C50;
static inline int CheckRangeOut(ObjectMaster* obj)
{
	int result;
	__asm
	{
		mov edx, [obj]
		call CheckRangeOutPtr
		mov result, eax
	}
	return result;
}