#include "stdafx.h"
#include "helper.h"

// Very hacky stuff to allow unique animations for Tails

static constexpr int animcount = 300;
static constexpr int bytecount = sizeof(AnimationIndex) * animcount;

static AnimationIndex BackupAnimations[animcount]{};
static AnimationIndex TailsAnimations[animcount]{};

extern ObjectMaster* pNpcMilesTask;

static AnimationIndex* LoadTailsMTN_r(const char* name)
{
	if (pNpcMilesTask)
	{
		WriteData((NJS_MOTION***)0x750A94, &TailsAnimations[0].Animation);
		WriteData((NJS_MOTION***)0x750182, &TailsAnimations[0].Animation);

		memcpy(BackupAnimations, CharacterAnimations, bytecount);
		auto list = LoadMTNFile((char*)name);
		memcpy(TailsAnimations, CharacterAnimations, bytecount);
		memcpy(CharacterAnimations, BackupAnimations, bytecount);
		return list;
	}
	else
	{
		WriteData((NJS_MOTION***)0x750A94, &CharacterAnimations[0].Animation);
		WriteData((NJS_MOTION***)0x750182, &CharacterAnimations[0].Animation);
		return LoadMTNFile((char*)name);
	}
}

static void __declspec(naked) LoadTailsMTN_asm()
{
	__asm
	{
		push eax
		call LoadTailsMTN_r
		add esp, 4
		retn
	}
}

static void __cdecl PSetTailsMotion_r(CharAnimInfo* mjp)
{
	if (pNpcMilesTask) // todo: rewrite
	{
		memcpy(BackupAnimations, CharacterAnimations, bytecount);
		memcpy(CharacterAnimations, TailsAnimations, bytecount);
		PSetMotion(mjp);
		memcpy(TailsAnimations, CharacterAnimations, bytecount);
		memcpy(CharacterAnimations, BackupAnimations, bytecount);
	}
	else
	{
		PSetMotion(mjp);
	}
}

static void __declspec(naked) PSetTailsMotion_asm()
{
	__asm
	{
		push esi
		call PSetTailsMotion_r
		pop esi
		retn
	}
}

void PatchAnimations()
{
	WriteCall(reinterpret_cast<void*>(0x74D02B), LoadTailsMTN_asm);
	WriteCall(reinterpret_cast<void*>(0x74D352), PSetTailsMotion_asm);
	WriteCall(reinterpret_cast<void*>(0x74D03C), PSetTailsMotion_asm);
}