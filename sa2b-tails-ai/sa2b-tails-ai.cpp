#include "stdafx.h"

Trampoline* LoadCharacters_t = nullptr;

enum class AIActions {
	Start,
	Unknown1,
	Respawn,
	Fly,
	Run,
	PlayerControl,
	Unknown6,
	Unknown7,
	Unknown8,
};

enum class AISubActions {
	Init,
	Normal,
	Run,
	Unknown3,
	Unknown4,
	Unknown5
};

struct TailsAI {
	AIActions action;
	AISubActions subaction;
	NJS_VECTOR pos_backup;
	Rotation rot_backup;
	int timer_rangeout;
	int timer_nomove;
};

TailsAI* TailsAIWorker = nullptr;

// AI functions

void SetTailsFlying(EntityData1* twk, motionwk* pwk, CharObj2Base* co2) {
	pwk->spd.z = 0.0;
	pwk->spd.y = 0.0;
	pwk->spd.x = 0.0;
	co2->Speed.z = 0.0;
	co2->Speed.y = 0.0;
	co2->Speed.x = 0.0;
	ForcePlayerAction(twk, GeneralAction_Fly);
}

float AI_RangeMax(float xmag) {
	if (xmag <= 1.0f) {
		if (xmag < 0.1f) {
			xmag = 0.0f;
		}
	}
	else {
		xmag = 1.0f;
	}

	return xmag;
}

void AI_BrakeVelocity(NJS_VECTOR* spd, float* xmag, Angle* rot) {
	if (spd->x != 0.0 || spd->z != 0.0) {
		if (xmag) {
			*xmag = 1.0;
		}

		if (rot) {
			*rot = 0x8000 - (unsigned __int64)(atan2(spd->z, spd->x) * 65536.0 * -0.1591549762031479);
		}
	}
	else {
		if (xmag) {
			*xmag = 0.0;
		}

		if (rot) {
			*rot = 0;
		}
	}
}

bool AI_RangeOut(EntityData1* p1, EntityData1* p2) {
	float dist = GetDistance(&p1->Position, &p2->Position);

	if (dist > 1000.0f) {
		return true;
	}

	if (dist >= 50.0f) {
		// screen projection here
		return false;
	}

	return false;
}

void CharacterAI_WriteAnalog(TailsAI* aiwk, EntityData1* playertwk, motionwk* playermwk, CharObj2Base* playerco2, EntityData1* leadertwk) {
	int playerid = playerco2->PlayerNum;
	int leaderid = 0;

	CharObj2Base* leadco2 = MainCharObj2[leaderid];

	NJS_VECTOR* aipos = &playertwk->Position;
	NJS_VECTOR* leadpos = &leadertwk->Position;

	NJS_VECTOR diff = { aipos->x - leadpos->x,  0, aipos->z - leadpos->z};
	njUnitVector(&diff);

	float xmag = diff.x * 5.0 + leadpos->x - aipos->x;
	float zmag = diff.z * 5.0 + leadpos->z - aipos->z;
	float mag = sqrtf(xmag * xmag + zmag * zmag);

	Angle angle = atan2f(zmag, xmag) * 65536.0 * 0.1591549762031479;

	xmag = 0.0f;
	double v12 = 0.0f;
	int pressed = 0;
	int held = 0;

	switch (aiwk->subaction) {
	case AISubActions::Init:
		aiwk->subaction = AISubActions::Normal;
		break;
	case AISubActions::Normal:
		if (mag >= 15.0f) {
			if (mag >= 40.0f) {
				xmag = 1.0f;
			}
			else {
				xmag = (mag - 20.0f) * 0.025f + 0.5f;
			}
		}
		else if (leadco2->Speed.x >= (double)leadco2->PhysData.RollCancel || playerco2->Speed.x <= (double)playerco2->PhysData.RollEnd) {
			if (playerco2->Speed.x == 0.0f && mag >= 1.0f) {
				xmag = 0.0049999999f;
			}
			else {
				xmag = 0.0f;
			}
		}
		else {
			AI_BrakeVelocity(&playermwk->spd, &xmag, &angle);
		}

		if (playertwk->Status & (Status_Unknown1 | Status_Ground)) {
			if (leadpos->y - aipos->y > 20.0f && leadco2->Speed.x < leadco2->PhysData.RollEnd) {
				pressed |= JumpButtons;
				aiwk->subaction = AISubActions::Run;
			}
		}

		break;
	case AISubActions::Run:
		xmag = AI_RangeMax(mag * 0.05f);

		if (leadpos->y + 3.0f > aipos->y) {
			if (playermwk->spd.y >= 0.0f) {
				held |= JumpButtons;
			}
			else {
				pressed = JumpButtons | Controllers[playerid].press;
				aiwk->subaction = AISubActions::Unknown3;
			}
		}

		if (playertwk->Status & (Status_Unknown1 | Status_Ground) && !(playertwk->Status & Status_Ball)) {
			aiwk->subaction = AISubActions::Normal;
		}

		break;
	case AISubActions::Unknown3:
		xmag = AI_RangeMax(mag * 0.05f);

		if (playertwk->Status & (Status_Unknown1 | Status_Ground)) {
			aiwk->subaction = AISubActions::Normal;
		}
		else if (mag >= 20.0f || leadpos->y >= aipos->y) {
			held |= JumpButtons;
		}
		else {
			pressed = AttackButtons | Controllers[playerid].press;
		}

		break;
		case AISubActions::Unknown4:
			xmag = AI_RangeMax(mag * 0.05f);

			if (playertwk->Status & (Status_Unknown1 | Status_Ground)) {
				aiwk->subaction = AISubActions::Normal;
			}
			else if (playertwk->Position.y - leadpos->y < 20.0f)
			{
				aiwk->subaction = AISubActions::Unknown3;
				pressed |= JumpButtons;
				held |= JumpButtons;
			}

			break;
		case AISubActions::Unknown5:
			diff.x = leadpos->x - aipos->x;
			diff.y = leadpos->y - aipos->y;
			diff.z = leadpos->z - aipos->z;

			if (njScalor(&diff) < 50.0f || playertwk->Status & Status_Hurt) {
				aiwk->subaction = AISubActions::Normal;
				held |= AttackButtons;
			}
			else {
				normalizevector(&diff, &diff);
				aipos->x += diff.x;
				aipos->y += diff.y;
				aipos->z += diff.z;
			}
			break;
	default:
		return;
	}

	if (GetDistance(&aiwk->pos_backup, aipos) >= 1.0f) {
		aiwk->timer_nomove = 0;
	}
	else if (++aiwk->timer_nomove > 60 && mag > 30.0f) {
		aiwk->timer_nomove = 0;
		pressed |= JumpButtons;
		aiwk->subaction = AISubActions::Run;
	}

	aiwk->pos_backup = playertwk->Position;
	aiwk->rot_backup = playertwk->Rotation;

	Controllers[playerid].press |= pressed;
	Controllers[playerid].on |= held;

	AnalogThings[playerid].direction = angle;
	AnalogThings[playerid].magnitude = xmag;

	if (pressed & JumpButtons) {
		Jump_Pressed[playerid] = true;
	}
	
	if (held & JumpButtons) {
		Jump_Held[playerid] = true;
	}

	if (pressed & AttackButtons) {
		Action_Pressed[playerid] = true;
	}

	if (held & AttackButtons) {
		Action_Held[playerid] = true;
	}
}

void TailsAI_Main(TailsAI* aiwk, EntityData1* playertwk, motionwk* playermwk, CharObj2Base* playerco2) {
	int leaderid = 0; // Player the AI follows
	int playerid = playerco2->PlayerNum;

	EntityData1* leadertwk = MainCharObj1[leaderid];

	switch (aiwk->action) {
	case AIActions::Start: // Place the AI behind the leader
		DisableController(playerid);
		aiwk->timer_rangeout = 0;
		GetPlayerSidePos(&playertwk->Position, leadertwk, 10.0);
		SavePlayerPosition(playerid, 0, &playertwk->Position, 0);
		aiwk->action = AIActions::Run;
		aiwk->subaction = AISubActions::Init;
		break;
	case AIActions::Unknown1:
		playertwk->Position = leadertwk->Position;
		playertwk->Position.y += 1000000.0f;
		SavePlayerPosition(playerid, 0, &playertwk->Position, 0);
		break;
	case AIActions::Respawn: // Respawn after being too far
		DisableController(playerid);
		SetToCameraPosition(&playertwk->Position);
		playertwk->Position.y = leadertwk->Position.y + 50.0f;
SavePlayerPosition(playerid, 0, &playertwk->Position, 0);
SetTailsFlying(playertwk, playermwk, playerco2);
Controllers[playerid].on |= Buttons_A;
Controllers[playerid].press |= Buttons_A;

aiwk->action = AIActions::Run;
aiwk->subaction = AISubActions::Unknown3;
break;
	case AIActions::Fly:
		DisableController(playerid);
		SetTailsFlying(playertwk, playermwk, playerco2);
		Controllers[playerid].on |= Buttons_A;
		Controllers[playerid].press |= Buttons_A;

		aiwk->subaction = AISubActions::Unknown3;
		break;
	case AIActions::Run: // Run the main movements

		// If the player controller has input, deactive tails AI temporarily
		if (Controllers[playerid].on & (Buttons_L | Buttons_R | Buttons_X | Buttons_Y | Buttons_Right | Buttons_Left | Buttons_Down | Buttons_Up | Buttons_A | Buttons_B)) {
			EnableController(playerid);
			aiwk->timer_rangeout = 0;
			aiwk->action = AIActions::PlayerControl;
		}
		else {
			DisableController(playerid);
		}

		CharacterAI_WriteAnalog(aiwk, playertwk, playermwk, playerco2, leadertwk);

		// Too far away
		if (AI_RangeOut(playertwk, leadertwk)) {
			aiwk->action = AIActions::Respawn;
		}

		break;
	case AIActions::PlayerControl: // let the player be moved by controller, reactive AI if 300 frames elapsed
		if (Controllers[playerid].on & (Buttons_L | Buttons_R | Buttons_X | Buttons_Y | Buttons_Right | Buttons_Left | Buttons_Down | Buttons_Up | Buttons_A | Buttons_B)) {
			EnableController(playerid);
			aiwk->timer_rangeout = 0;
		}
		else {
			if (++aiwk->timer_rangeout > 300) {
				DisableController(playerid);
				aiwk->timer_rangeout = 0;
				aiwk->action = AIActions::Run;
			}
		}

		// Too far away
		if (AI_RangeOut(playertwk, leadertwk)) {
			aiwk->timer_rangeout = 0;
			aiwk->action = AIActions::Respawn;
		}

		break;
	case AIActions::Unknown6:
		playertwk->Position = leadertwk->Position;
		playertwk->Position.y += 1000000.0f;
		SavePlayerPosition(playerid, 0, &playertwk->Position, 0);
		aiwk->action = AIActions::Start;
		break;
	case AIActions::Unknown7:
		playertwk->Position = leadertwk->Position;
		playertwk->Position.y += 1000000.0f;
		SavePlayerPosition(playerid, 0, &playertwk->Position, 0);
		aiwk->timer_rangeout += 1;

		if (aiwk->timer_rangeout > 3) {
			ForcePlayerAction(playertwk, GeneralActions_12);
			aiwk->action = AIActions::Unknown8;
		}
		break;
	case AIActions::Unknown8:
		playertwk->Position = leadertwk->Position;
		playertwk->Position.y += 1000000.0f;
		SavePlayerPosition(playerid, 0, &playertwk->Position, 0);

		// SADX checks if TailsAI exists within TailsAI here, we don't.
		ForcePlayerAction(playertwk, GeneralActions_24);
		DisableController(1u);
		aiwk->action = AIActions::Respawn;

		break;
	default:
		return;
	}
}

// Tails' Object with the AI:

void __cdecl TailsWithAI_Main(ObjectMaster* obj) {
	TailsAI_Main((TailsAI*)obj->field_4C, obj->Data1.Entity, (motionwk*)obj->EntityData2, obj->Data2.Character);
	Tails_Main(obj);

	CameraScreensInfoArray[1] = CameraScreensInfoArray[0];
}

void __cdecl TailsWithAI_Delete(ObjectMaster* obj) {
	delete obj->field_4C;
}

void LoadTailsWithAI(int playerid) {
	LoadTails(1);

	TailsAIWorker = new TailsAI();

	MainCharacter[playerid]->field_4C = TailsAIWorker;
	MainCharacter[playerid]->MainSub = TailsWithAI_Main;
	MainCharacter[playerid]->DeleteSub = TailsWithAI_Delete;
}

void LoadCharacters_r() {
	NonStaticFunctionPointer(void, Original, (), LoadCharacters_t->Target());
	
	// Load Tails AI if in a Sonic level
	if (TwoPlayerMode == false) {
		if (CurrentCharacter == Characters_Sonic) {
			LoadTailsWithAI(1);
			WriteData<1>((char*)0x46B02E, (char)0x01); // Deathzone only for P1
		}
	}
	else {
		WriteData<1>((char*)0x46B02E, (char)0x02); // Restore DeathZone
	}

	Original();
}

// Fix functions

int __cdecl DamagePlayer_r(EntityData1* data1, CharObj2Base* data2) {
	if (TailsAIWorker && data2->PlayerNum == 1 && data2->CharID == Characters_Tails) {
		return 0;
	}
	
	return DamagePlayer(data1, data2);
}

void __cdecl RemoveTailsVoice(int idk, int num) {
	if (TailsAIWorker) {
		return;
	}
	
	PlayVoice(idk, num);
}

static void __declspec(naked) PlayVoiceAsm(int idk, int num) {
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

void __cdecl RemoveTailsSound(int ID, int Entity, char Bank, char Volume) {
	if (TailsAIWorker) {
		return;
	}
	
	PlaySoundProbably(ID, Entity, Bank, Volume);
}

static void __declspec(naked) PlaySoundAsm(int ID, int Entity, char Bank, char Volume) {
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

extern "C" {
	__declspec(dllexport) void Init() {
		LoadCharacters_t = new Trampoline((intptr_t)LoadCharacters, (intptr_t)LoadCharacters + 0x6, LoadCharacters_r);
		WriteCall((void*)0x74dc9c, DamagePlayer_r);

		////Remove Tails voice when AI
		WriteCall((void*)0x751C90, PlayVoiceAsm);
		WriteCall((void*)0x752DE1, PlayVoiceAsm);

		////Remove Tails Sound Effects when AI
		WriteCall((void*)0x751C7E, PlaySoundAsm);
		WriteCall((void*)0x74EB6A, PlaySoundAsm);
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}