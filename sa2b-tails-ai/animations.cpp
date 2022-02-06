#include "stdafx.h"
#include "helper.h"

static constexpr int animcount = 300;
static constexpr int bytecount = sizeof(AnimationIndex) * animcount;

static AnimationIndex CmnAnimations[animcount]{};
static AnimationIndex TailsAnimations[animcount]{};

AnimationIndex* LoadCmnMTN_(const char* name)
{
	auto mem = LoadMTNFile((char*)"PLCOMMTN.PRS");
	memcpy(CmnAnimations, CharacterAnimations, bytecount);
	return mem;
}

static void __declspec(naked) LoadCmnMTN()
{
	__asm
	{
		push eax
		call LoadCmnMTN_
		add esp, 4
		retn
	}
}

AnimationIndex* LoadTailsMTN_(const char* name)
{
	AnimationIndex temp[animcount];
	memcpy(temp, CharacterAnimations, bytecount);
	memcpy(CharacterAnimations, CmnAnimations, bytecount);
	auto mem = LoadMTNFile((char*)name);
	memcpy(TailsAnimations, CharacterAnimations, bytecount);
	memcpy(CharacterAnimations, temp, bytecount);
	return mem;
}

static void __declspec(naked) LoadTailsMTN()
{
	__asm
	{
		push eax
		call LoadTailsMTN_
		add esp, 4
		retn
	}
}

void PatchAnimations()
{
	WriteData((NJS_MOTION***)0x750A94, &TailsAnimations[0].Animation);
	WriteData((NJS_MOTION***)0x750182, &TailsAnimations[0].Animation);
	WriteCall((void*)0x74D02B, LoadTailsMTN);
	WriteCall((void*)0x45946E, LoadCmnMTN);
}