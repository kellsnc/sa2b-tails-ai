#pragma once

#define TARGET_STATIC(name) ((decltype(name##_r)*)name##_t.Target())
#define STATUS_FLOOR (Status_OnObjectColli | Status_Ground)

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

DataArray(motionwk*, playermwp, 0x1DE95E0, 8);
DataPointer(ObjectMaster*, pMiniEventTask, 0x19456A8);

enum GeneralActions {
	GeneralActions_ObjectCtrl = 9, // SADX: 12
	GeneralActions_Reset = 15, // SADX: 24
	GeneralAction_Fly = 29 // SADX: 48
};

static Buttons JumpButtons = Buttons_A;
static Buttons AttackButtons = Buttons_X;

void SetInputP_(EntityData1* data, GeneralActions gaction);
void PadReadOffP_(int index);
void PadReadOnP_(int index);
void GetPlayerSidePos(NJS_VECTOR* pos, EntityData1* entity, float dist);
void SetToCameraPosition(NJS_VECTOR* pos, int pnum);
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest);
bool SetPlayerPosition_(int playerNum, int unk, NJS_VECTOR* pos, Rotation* rot);
float ghUnitVector_(NJS_VECTOR* vo, NJS_VECTOR* vd);

static const void* const PSetMotionPtr = (void*)0x4692A0;
static inline void PSetMotion(CharAnimInfo* mjp)
{
	__asm
	{
		mov esi, [mjp]
		call PSetMotionPtr
	}
}