#include "stdafx.h"

Trampoline* LoadCharacters_t = nullptr;

enum AIActions {
	AI_Start,
	AI_Run
};

void CharacterAI_WriteAnalog(EntityData1* data, EntityData1* playerwk, CharObj2Base* playerco2) {
	// do movements here
}

void CharacterAI(ObjectMaster* obj) {
	EntityData1* data = obj->Data1.Entity;
	EntityData1* playerwk = MainCharObj1[data->Index];
	CharObj2Base* playerco2 = MainCharObj2[data->Index];

	switch (data->Action) {
	case AI_Start:
		ControllerEnabled[data->Index] = false;
		data->Action = AI_Run;
		break;
	case AI_Run:
		CharacterAI_WriteAnalog(data, playerwk, playerco2);
		break;
	}
}

void LoadCharacterAI(int player) {
	ObjectMaster* aitask = LoadObject(1, "CharacterAI", CharacterAI, LoadObj_Data1);
	EntityData1* data = aitask->Data1.Entity;

	data->Index = player;
	data->Action = AI_Start;
}

void LoadCharacters_r() {
	NonStaticFunctionPointer(void, Original, (), LoadCharacters_t->Target());

	if (CurrentCharacter == Characters_Sonic) {
		LoadTails(1); // if you load tails first, no animation issue somehow
		LoadCharacterAI(1);
	}

	Original();
}

extern "C"
{
	__declspec(dllexport) void Init() {
		LoadCharacters_t = new Trampoline((intptr_t)LoadCharacters, (intptr_t)LoadCharacters + 0x6, LoadCharacters_r);
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}