#include "stdafx.h"
#include "SA2ModLoader.h"
#include "tails-ai.h"
#include "animations.h"

static BOOL __cdecl DamagePlayer_r(EntityData1* data1, CharObj2Base* data2)
{
	if (pNpcMilesTask && data2->PlayerNum == 1 && data2->CharID == Characters_Tails)
	{
		return FALSE;
	}

	return DamagePlayer(data1, data2);
}

static void __cdecl RemoveTailsVoice(int idk, int num)
{
	if (!pNpcMilesTask)
	{
		PlayVoice(idk, num);
	}
}

static void __declspec(naked) PlayVoiceAsm()
{
	__asm
	{
		push[esp + 04h]
		push edx
		call RemoveTailsVoice
		pop edx
		add esp, 4
		retn
	}
}

static void __cdecl RemoveTailsSound(int ID, int Entity, char Bank, char Volume)
{
	if (!pNpcMilesTask)
	{
		PlaySoundProbably(ID, Entity, Bank, Volume);
	}
}

static void __declspec(naked) PlaySoundAsm()
{
	__asm
	{
		push[esp + 0Ch] // Volume
		push[esp + 0Ch] // Bank
		push[esp + 0Ch] // Entity
		push esi // ID
		call RemoveTailsSound
		pop esi
		add esp, 12
		retn
	}
}

extern "C"
{
	__declspec(dllexport) void Init()
	{
		// Prevent Tails AI from hurting player
		WriteCall((void*)0x74dc9c, DamagePlayer_r);

		// Remove Tails voice when AI
		WriteCall((void*)0x751C90, PlayVoiceAsm);
		WriteCall((void*)0x752DE1, PlayVoiceAsm);

		// Remove Tails Sound Effects when AI
		WriteCall((void*)0x751C7E, PlaySoundAsm);
		WriteCall((void*)0x74EB6A, PlaySoundAsm);

		PatchAnimations();
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}