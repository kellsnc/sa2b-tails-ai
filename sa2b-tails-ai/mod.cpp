#include "stdafx.h"
#include "SA2ModLoader.h"
#include "tails-ai.h"

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

void __cdecl PGetRotation_r(EntityData1* twp, motionwk* mwp, CharObj2Base* pwp);
Trampoline PGetRotation_t(0x45FA70, 0x45FA75, PGetRotation_r);
void __cdecl PGetRotation_r(EntityData1* twp, motionwk* mwp, CharObj2Base* pwp)
{
	// Fix crashes caused by accelerating surfaces because no camera exist for AI
	if (twp->field_2 == ID_NPC && (int)pwp->CurrentSurfaceFlags & SurfaceFlag_Accelerate)
	{
		*(int*)&pwp->CurrentSurfaceFlags &= ~SurfaceFlag_Accelerate;
	}

	TARGET_STATIC(PGetRotation)(twp, mwp, pwp);
}

void __cdecl PGetAcceleration_r(EntityData1* twp, motionwk* mwp, CharObj2Base* pwp);
Trampoline PGetAcceleration_t(0x45B610, 0x45B616, PGetAcceleration_r);
void __cdecl PGetAcceleration_r(EntityData1* twp, motionwk* mwp, CharObj2Base* pwp)
{
	// Fix crashes caused by accelerating surfaces because no camera exist for AI
	if (twp->field_2 == ID_NPC && (int)pwp->CurrentSurfaceFlags & SurfaceFlag_Accelerate)
	{
		*(int*)&pwp->CurrentSurfaceFlags &= ~SurfaceFlag_Accelerate;
	}

	TARGET_STATIC(PGetAcceleration)(twp, mwp, pwp);
}


extern "C"
{
	__declspec(dllexport) void Init()
	{
		HMODULE SA2Anim = GetModuleHandle(L"SA2-Anim-Break");

		if (!SA2Anim)
		{
			MessageBox(MainWindowHandle,
				L"Failed to load SA2-Anim-Break.dll, the mod won't work!\nPlease install SA2 Anim Break mod.", L"SA2 Tails AI: DLL not found!", MB_OK | MB_ICONERROR);
		}

		Init_MilesNPC();

		// Prevent Tails AI from hurting player
		WriteCall((void*)0x74dc9c, DamagePlayer_r);

		// Remove Tails voice when AI
		WriteCall((void*)0x751C90, PlayVoiceAsm);
		WriteCall((void*)0x752DE1, PlayVoiceAsm);

		// Remove Tails Sound Effects when AI
		WriteCall((void*)0x751C7E, PlaySoundAsm);
		WriteCall((void*)0x74EB6A, PlaySoundAsm);
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}