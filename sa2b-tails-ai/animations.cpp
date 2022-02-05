#include "stdafx.h"
#include "helper.h"

static constexpr int animcount = 300;
static constexpr int bytecount = sizeof(AnimationIndex) * animcount;

static AnimationIndex TailsAnimations[animcount]{};

void LoadMTNFile_(const char* name, int playernum)
{
	AnimationIndex temp[animcount];
	memcpy(temp, CharacterAnimations, bytecount);
	memset(CharacterAnimations, 0, bytecount);
	LoadMTNFile((char*)"PLCOMMTN.PRS");
	LoadMTNFile((char*)name);
	memcpy(TailsAnimations, CharacterAnimations, bytecount);
	memcpy(CharacterAnimations, temp, bytecount);
}

static void __declspec(naked) LoadTailsMTN()
{
	__asm
	{
		push eax
		call LoadMTNFile_
		pop eax
		retn
	}
}

void PatchAnimations()
{
	WriteData((NJS_MOTION***)0x750A94, &TailsAnimations[0].Animation);
	WriteData((NJS_MOTION***)0x750182, &TailsAnimations[0].Animation);
	WriteCall((void*)0x74D02B, LoadTailsMTN);
}