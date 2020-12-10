#include "stdafx.h"

Trampoline* LoadCharacters_t = nullptr;

ObjectMaster* TailsAI_Ptr = nullptr;

enum AIActions {
	AI_Start,
	AI_Unknown1,
	AI_Respawn,
	AI_Fly,
	AI_Run,
	AI_PlayerControl,
	AI_Unknown6,
	AI_Unknown7,
	AI_Unknown8,
};

enum AISubActions {
	AISub_Init,
	AISub_Normal,
	AISub_Walk,
	AISub_Unknown3,
	AISub_Unknown4,
	AISub_Unknown5
};

void SetTailsFlying(EntityData1* twk, motionwk* pwk) {
	pwk->spd.z = 0.0;
	pwk->spd.y = 0.0;
	pwk->spd.x = 0.0;
	pwk->work->Speed.z = 0.0;
	pwk->work->Speed.y = 0.0;
	pwk->work->Speed.x = 0.0;
	ForcePlayerAction(twk, GeneralAction_Fly);
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

void CharacterAI_WriteAnalog(EntityData1* data, EntityData1* playertwk, motionwk* playermwk, EntityData1* leadertwk) {
	int playerid = data->Index;
	int leaderid = data->field_2;

	CharObj2Base* co2 = playermwk->work;
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

	int pressed = 0;
	int held = 0;

	switch ((AISubActions)data->NextAction) {
	case AISub_Init:
		data->NextAction = AISub_Normal;
		break;
	case AISub_Normal:

		if (mag >= 15.0f) {
			if (mag >= 40.0f) {
				xmag = 1.0f;
			}
			else {
				xmag = (mag - 20.0f) * 0.025f + 0.5f;
			}
		}
		else if (leadco2->Speed.x >= (double)leadco2->PhysData.RollCancel || co2->Speed.x <= (double)co2->PhysData.RollEnd) {
			if (co2->Speed.x == 0.0f && mag >= 1.0f) {
				xmag = 0.0049999999f;
			}
			else {
				xmag = 0.0f;
			}
		}
		else
		{
			//sub_47DE00(&EntityData2Ptrs[1]->VelocityDirection, &xmag, &angle);
			xmag = 1.0f;
		}

		if (playertwk->Status & (Status_Unknown1 | Status_Ground)) {
			if (leadpos->y - aipos->y > 20.0f && leadco2->Speed.x < leadco2->PhysData.RollEnd) {
				pressed |= JumpButtons;
				data->NextAction = AISub_Walk;
			}
		}

		break;
	case AISub_Walk:
		break;
	default:
		return;
	}

	if (GetDistance(&data->Position, aipos) >= 1.0f) {
		data->Status = 0;
	}

	else if (++data->Status > 60 && mag > 30.0f)
	{
		data->Status = 0;
		pressed |= JumpButtons;
		data->NextAction = AISub_Walk;
	}

	data->Position = *aipos;
	data->Rotation = playertwk->Rotation;

	Controllers[playerid].press |= pressed;
	Controllers[playerid].on |= held;

	AnalogThings[playerid].direction = angle;
	AnalogThings[playerid].magnitude = xmag;
}

void CharacterAI_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1.Entity;

	// AI Player
	EntityData1* playertwk = MainCharObj1[data->Index];
	motionwk* playermwk = MainCharMotions[data->Index]; // contains movement, and CharObj2Base (phyiscs, anim...)
	playermwk->work = MainCharObj2[data->Index];
	int playerid = data->Index;

	// Player the AI follows
	EntityData1* leadertwk = MainCharObj1[data->field_2];
	int leaderid = data->field_2;

	switch ((AIActions)data->Action) {
	case AI_Start: // Place the AI behind the leader
		DisableController(playerid);
		GetPlayerSidePos(&playertwk->Position, leadertwk, 10.0);
		data->Action = AI_Run;
		data->NextAction = AISub_Init;
		break;
	case AI_Unknown1:
		playertwk->Position = leadertwk->Position;
		playertwk->Position.y += 1000000.0f;
		break;
	case AI_Respawn: // Respawn after being too far
		DisableController(playerid);
		SetToCameraPosition(&playertwk->Position);
		playertwk->Position.y = leadertwk->Position.y + 50.0f;
		SetTailsFlying(playertwk, playermwk);
		Controllers[playerid].on |= Buttons_A;
		Controllers[playerid].press |= Buttons_A; 
		data->NextAction = AISub_Unknown3;
		data->Action = AI_Run;
		break;
	case AI_Fly:
		DisableController(playerid);
		SetTailsFlying(playertwk, playermwk);
		Controllers[playerid].on |= Buttons_A;
		Controllers[playerid].press |= Buttons_A;
		data->NextAction = AISub_Unknown3;
		break;
	case AI_Run: // Run the main movements

		// If the player controller has input, deactive tails AI temporarily
		if (Controllers[playerid].on & (Buttons_L | Buttons_R | Buttons_X | Buttons_Y | Buttons_Right | Buttons_Left | Buttons_Down | Buttons_Up | Buttons_A | Buttons_B)) {
			EnableController(playerid);
			data->field_6 = 0;
			data->Action = AI_PlayerControl;
		}
		else {
			DisableController(playerid);
		}
		
		CharacterAI_WriteAnalog(data, playertwk, playermwk, leadertwk);
		
		// Too far away
		if (AI_RangeOut(playertwk, leadertwk)) {
			data->Action = AI_Respawn;
		}

		break;
	case AI_PlayerControl: // let the player be moved by controller, reactive AI if 300 frames elapsed
		if (Controllers[playerid].on & (Buttons_L | Buttons_R | Buttons_X | Buttons_Y | Buttons_Right | Buttons_Left | Buttons_Down | Buttons_Up | Buttons_A | Buttons_B)) {
			EnableController(playerid);
			data->field_6 = 0;
		}
		else {
			if (++data->field_6 > 300) {
				DisableController(playerid);
				data->field_6 = 0;
				data->Action = AI_Run;
			}
		}

		// Too far away
		if (AI_RangeOut(playertwk, leadertwk)) {
			data->field_6 = 0;
			data->Action = AI_Respawn;
		}

		break;
	case AI_Unknown6:
		break;
	case AI_Unknown7:
		break;
	case AI_Unknown8:
		break;
	default:
		return;
	}
}

void CharacterAI_Delete(ObjectMaster* obj) {
	TailsAI_Ptr = nullptr;
}

void LoadCharacterAI(int player, int leader) {
	ObjectMaster* aitask = LoadObject(1, "CharacterAI", CharacterAI_Main, LoadObj_Data1);
	EntityData1* data = aitask->Data1.Entity;

	aitask->DeleteSub = CharacterAI_Delete;

	data->Index = player;
	data->field_2 = leader;
	data->Action = AI_Start;

	TailsAI_Ptr = aitask;
}

void LoadCharacters_r() {
	NonStaticFunctionPointer(void, Original, (), LoadCharacters_t->Target());

	if (TwoPlayerMode == false) {
		if (CurrentCharacter == Characters_Sonic) {
			LoadTails(1); // If you load tails first, no animation issue somehow
			LoadCharacterAI(1, 0);
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