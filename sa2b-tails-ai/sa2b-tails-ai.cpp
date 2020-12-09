#include "stdafx.h"

Trampoline* LoadCharacters_t = nullptr;

ObjectMaster* TailsAI_Ptr = nullptr;

enum AIActions {
	AI_Start,
	AI_Run,
	Ai_Respawn
};

void CharacterAI_WriteAnalog(EntityData1* data, EntityData1* playerwk, CharObj2Base* playerco2) {
	// do movements here
}

void CharacterAI_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1.Entity;
	EntityData1* playerwk = MainCharObj1[data->Index];
	CharObj2Base* playerco2 = MainCharObj2[data->Index];
	EntityData1* p1 = MainCharObj1[0];

	switch (data->Action) {
	case AI_Start:
		DisableController(data->Index);
		GetPlayerSidePos(&playerwk->Position, p1, 10.0);
		data->Action = 4;
		data->NextAction = 0;
		break;
	case AI_Run:
		CharacterAI_WriteAnalog(data, playerwk, playerco2);
		break;
	case Ai_Respawn:
		DisableController(data->Index);
		SetToCameraPosition(&playerwk->Position);
		playerwk->Position.y = p1->Position.y + 50.0;
		Controllers[1].press |= Buttons_A; 
		data->NextAction = 3;
		data->Action = 4;
		break;
	case 3:
		DisableController(data->Index);
		Controllers[1].press |= Buttons_A;
		data->NextAction = 3;
		break;
	case 4:
		CharacterAI_WriteAnalog(data, playerwk, playerco2);
		DisableController(data->Index);
		break;
	case 5:
		DisableController(data->Index);
		//rangeout check here
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	default:
		return;
	}
}

void CharacterAI_Delete(ObjectMaster* obj) {
	TailsAI_Ptr = nullptr;
}

void LoadCharacterAI(int player) {
	ObjectMaster* aitask = LoadObject(1, "CharacterAI", CharacterAI_Main, LoadObj_Data1);
	EntityData1* data = aitask->Data1.Entity;

	aitask->DeleteSub = CharacterAI_Delete;

	data->Index = player;
	data->Action = AI_Start;

	TailsAI_Ptr = aitask;
}

void LoadCharacters_r() {
	NonStaticFunctionPointer(void, Original, (), LoadCharacters_t->Target());

	if (TwoPlayerMode == false) {
		if (CurrentCharacter == Characters_Sonic) {
			LoadTails(1); // If you load tails first, no animation issue somehow
			LoadCharacterAI(1);
			WriteData<1>((char*)0x46B02E, (char)0x01); // Deathzone only for P1
		}
	}
	else {
		WriteData<1>((char*)0x46B02E, (char)0x02); // Restore DeathZone
	}

	Original();
}

extern "C" {
	__declspec(dllexport) void Init() {
		LoadCharacters_t = new Trampoline((intptr_t)LoadCharacters, (intptr_t)LoadCharacters + 0x6, LoadCharacters_r);
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}