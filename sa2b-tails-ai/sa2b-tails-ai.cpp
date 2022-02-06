#include "stdafx.h"
#include "helper.h"
#include "animations.h"

Trampoline* SetPlayer_t = nullptr;

enum class AIActions {
	Start,
	Unknown1,
	Respawn,
	Fly,
	Run,
	PlayerControl,
	Wait,
	Unknown7,
	StandBy,
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
	int pnum;
	int tgtnum;
};

ObjectMaster* pNpcMilesTask = nullptr;
bool NpcMilesStandByFlag = false;

void SetPlayerFlyMode(EntityData1* twp, motionwk* mwp, CharObj2Base* pwp)
{
	mwp->spd.z = 0.0f;
	mwp->spd.y = 0.0f;
	mwp->spd.x = 0.0f;
	pwp->Speed.z = 0.0f;
	pwp->Speed.y = 0.0f;
	pwp->Speed.x = 0.0f;
	SetInputP_(twp, GeneralAction_Fly);
}

float NpcClampRange(float xmag) // fake name
{
	if (xmag <= 1.0f)
	{
		if (xmag < 0.1f)
		{
			xmag = 0.0f;
		}
	}
	else
	{
		xmag = 1.0f;
	}

	return xmag;
}

void NpcGetAnalogForBrake(NJS_VECTOR* spd, float* xmag, Angle* rot)
{
	if (spd->x != 0.0 || spd->z != 0.0)
	{
		if (xmag)
		{
			*xmag = 1.0f;
		}

		if (rot)
		{
			*rot = 0x8000 - (unsigned __int64)(atan2(spd->z, spd->x) * 65536.0 * -0.1591549762031479);
		}
	}
	else
	{
		if (xmag)
		{
			*xmag = 0.0f;
		}

		if (rot)
		{
			*rot = 0;
		}
	}
}

void NpcMilesControl(ObjectMaster* tp)
{
	auto aiwk = (TailsAI*)tp->Data1.Entity;
	auto ptp = MainCharacter[aiwk->pnum];
	auto twp = ptp->Data1.Entity;
	auto mwp = (motionwk*)ptp->EntityData2;
	auto pwp = ptp->Data2.Character;
	auto target_twp = MainCharObj1[aiwk->tgtnum];
	auto target_pwp = MainCharObj2[aiwk->tgtnum];

	auto aipos = &twp->Position;
	auto leadpos = &target_twp->Position;

	NJS_VECTOR diff = { aipos->x - leadpos->x,  0, aipos->z - leadpos->z};
	njUnitVector(&diff);

	float xmag = diff.x * 5.0 + leadpos->x - aipos->x;
	float zmag = diff.z * 5.0 + leadpos->z - aipos->z;
	float mag = sqrtf(xmag * xmag + zmag * zmag);

	Angle angle = atan2f(zmag, xmag) * 65536.0 * 0.1591549762031479;

	xmag = 0.0f;
	int pressed = 0;
	int held = 0;

	switch (aiwk->subaction) {
	case AISubActions::Init:
		aiwk->subaction = AISubActions::Normal;
		break;
	case AISubActions::Normal:
		if (mag >= 15.0f)
		{
			if (mag >= 40.0f)
			{
				xmag = 1.0f;
			}
			else
			{
				xmag = (mag - 20.0f) * 0.025f + 0.5f;
			}
		}
		else if (target_pwp->Speed.x >= (double)target_pwp->PhysData.JogSpeed || pwp->Speed.x <= (double)pwp->PhysData.RunSpeed)
		{
			if (pwp->Speed.x == 0.0f && mag >= 1.0f)
			{
				xmag = 0.0045f;
			}
			else
			{
				xmag = 0.0f;
			}
		}
		else
		{
			NpcGetAnalogForBrake(&mwp->spd, &xmag, &angle);
		}

		if (twp->Status & STATUS_FLOOR)
		{
			if (leadpos->y - aipos->y > 20.0f && target_pwp->Speed.x < target_pwp->PhysData.RunSpeed)
			{
				pressed |= JumpButtons;
				aiwk->subaction = AISubActions::Run;
			}
		}

		break;
	case AISubActions::Run:
		xmag = NpcClampRange(mag * 0.05f);

		if (leadpos->y + 3.0f > aipos->y)
		{
			if (mwp->spd.y >= 0.0f)
			{
				held |= JumpButtons;
			}
			else
			{
				pressed = JumpButtons | Controllers[aiwk->pnum].press;
				aiwk->subaction = AISubActions::Unknown3;
			}
		}

		if (twp->Status & STATUS_FLOOR && !(twp->Status & Status_Ball))
		{
			aiwk->subaction = AISubActions::Normal;
		}

		break;
	case AISubActions::Unknown3:
		xmag = NpcClampRange(mag * 0.05f);

		if (twp->Status & STATUS_FLOOR)
		{
			aiwk->subaction = AISubActions::Normal;
		}
		else if (mag >= 20.0f || leadpos->y >= aipos->y)
		{
			held |= JumpButtons;
		}
		else
		{
			pressed = AttackButtons | Controllers[aiwk->pnum].press;
		}

		break;
		case AISubActions::Unknown4:
			xmag = NpcClampRange(mag * 0.05f);

			if (twp->Status & STATUS_FLOOR)
			{
				aiwk->subaction = AISubActions::Normal;
			}
			else if (twp->Position.y - leadpos->y < 20.0f)
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

			if (njScalor(&diff) < 50.0f || twp->Status & Status_Hurt)
			{
				aiwk->subaction = AISubActions::Normal;
				held |= AttackButtons;
			}
			else
			{
				ghUnitVector_(&diff, &diff);
				aipos->x += diff.x;
				aipos->y += diff.y;
				aipos->z += diff.z;
			}
			break;
	default:
		return;
	}

	if (GetDistance(&aiwk->pos_backup, aipos) >= 1.0f)
	{
		aiwk->timer_nomove = 0;
	}
	else if (++aiwk->timer_nomove > 60 && mag > 30.0f)
	{
		aiwk->timer_nomove = 0;
		pressed |= JumpButtons;
		aiwk->subaction = AISubActions::Run;
	}

	aiwk->pos_backup = twp->Position;
	aiwk->rot_backup = twp->Rotation;

	Controllers[aiwk->pnum].press |= pressed;
	Controllers[aiwk->pnum].on |= held;

	AnalogThings[aiwk->pnum].direction = angle;
	AnalogThings[aiwk->pnum].magnitude = xmag;

	if (pressed & JumpButtons)
	{
		Jump_Pressed[aiwk->pnum] = true;
	}
	
	if (held & JumpButtons)
	{
		Jump_Held[aiwk->pnum] = true;
	}

	if (pressed & AttackButtons)
	{
		Action_Pressed[aiwk->pnum] = true;
	}

	if (held & AttackButtons)
	{
		Action_Held[aiwk->pnum] = true;
	}
}

int NpcMilesRangeOut(EntityData1* twp1, EntityData1* twp2)
{
	float dist = GetDistance(&twp1->Position, &twp2->Position);

	if (dist > 1000.0f)
	{
		return true;
	}

	if (dist >= 50.0f)
	{
		// screen projection here
		return false;
	}

	return false;
}

void NpcMilesManageRangeOut(TailsAI* aiwk, EntityData1* twp1, EntityData1* twp2)
{
	auto out = NpcMilesRangeOut(twp1, twp2);

	if (out)
	{
		aiwk->timer_rangeout = 0;

		if (out == 2)
		{
			aiwk->action = AIActions::Fly;
		}
		else
		{
			aiwk->action = AIActions::Respawn;
			SetInputP_(twp1, GeneralActions_Reset);
			PadReadOffP_(aiwk->pnum);
		}
	}
}

void EV_NpcMilesStandByOn()
{
	NpcMilesStandByFlag = true;
}

void EV_NpcMilesStandByOff()
{
	NpcMilesStandByFlag = false;
}

bool NpcMilesCanBorn()
{
	if (NpcMilesStandByFlag || CurrentCharacter != Characters_Sonic || pMiniEventTask)
	{
		return false;
	}

	// Todo: blacklist levels

	return true;
}

void ComeBackNpcMiles(TailsAI* aiwk, EntityData1* twp)
{
	SetInputP_(twp, GeneralActions_Reset);
	PadReadOffP_(aiwk->pnum);
	aiwk->action = AIActions::Respawn;
}

void NpcMilesCustomPhysics(CharObj2Base* pwp)
{
	pwp->PhysData.AirAccel = 0.05f;
	pwp->PhysData.SpeedMaxH = 2.0f;
	pwp->PhysData.HangTime = 60;
	pwp->PhysData.JumpSpeed = 1.8f;
	pwp->PhysData.RunAccel = 0.05f;
	pwp->PhysData.AirResist = -0.008f;
	pwp->PhysData.DashSpeed = 5.09f;
}

void __cdecl Miles2PControl(ObjectMaster* tp)
{
	auto aiwk = (TailsAI*)tp->Data1.Entity;
	auto ptp = MainCharacter[aiwk->pnum];
	auto twp = ptp->Data1.Entity;
	auto mwp = (motionwk*)ptp->EntityData2;
	auto pwp = ptp->Data2.Character;
	auto target_twp = MainCharObj1[aiwk->tgtnum];

	// If tails AI should no longer be here
	if (!NpcMilesCanBorn())
	{
		SetInputP_(twp, GeneralActions_ObjectCtrl);
		aiwk->action = AIActions::Wait;
	}

	// Prevent AI from hurting you
	if (twp->Collision)
	{
		for (int i = 0; i < twp->Collision->Count; ++i)
		{
			twp->Collision->CollisionArray[i].damage &= ~0x20u;
		}
	}

	switch (aiwk->action) {
	case AIActions::Start: // Place the AI behind the leader
		PadReadOffP_(aiwk->pnum);
		aiwk->timer_rangeout = 0;
		GetPlayerSidePos(&twp->Position, target_twp, 10.0f); // put behind player
		SetPlayerPosition_(aiwk->pnum, 0, &twp->Position, 0);
		aiwk->action = AIActions::Run;
		aiwk->subaction = AISubActions::Init;
		NpcMilesCustomPhysics(pwp);
		break;
	case AIActions::Unknown1:
		twp->Position = target_twp->Position;
		twp->Position.y += 1000000.0f;
		SetPlayerPosition_(aiwk->pnum, 0, &twp->Position, 0);

		if (Controllers[aiwk->pnum].press & Buttons_Start && NpcMilesCanBorn())
		{
			ComeBackNpcMiles(aiwk, twp);
		}

		break;
	case AIActions::Respawn: // Respawn after being too far
		PadReadOffP_(aiwk->pnum);
		SetToCameraPosition(&twp->Position, aiwk->tgtnum);
		twp->Position.y = target_twp->Position.y + 50.0f;
		SetPlayerPosition_(aiwk->pnum, 0, &twp->Position, 0);
		SetPlayerFlyMode(twp, mwp, pwp);
		Controllers[aiwk->pnum].on |= Buttons_A;
		Controllers[aiwk->pnum].press |= Buttons_A;

		aiwk->action = AIActions::Run;
		aiwk->subaction = AISubActions::Unknown3;
		break;
	case AIActions::Fly:
		PadReadOffP_(aiwk->pnum);
		SetPlayerFlyMode(twp, mwp, pwp);
		Controllers[aiwk->pnum].on |= Buttons_A;
		Controllers[aiwk->pnum].press |= Buttons_A;

		aiwk->subaction = AISubActions::Unknown3;
		break;
	case AIActions::Run: // Run the main movements

		// If the player controller has input, deactive tails AI temporarily
		if (Controllers[aiwk->pnum].on & (Buttons_L | Buttons_R | Buttons_X | Buttons_Y | Buttons_Right | Buttons_Left | Buttons_Down | Buttons_Up | Buttons_A | Buttons_B))
		{
			PadReadOnP_(aiwk->pnum);
			aiwk->timer_rangeout = 0;
			aiwk->action = AIActions::PlayerControl;
		}
		else
		{
			PadReadOffP_(aiwk->pnum);
		}

		NpcMilesControl(tp);
		NpcMilesManageRangeOut(aiwk, twp, target_twp);

		break;
	case AIActions::PlayerControl: // let the player be moved by controller, reactive AI if 300 frames elapsed
		if (Controllers[aiwk->pnum].on & (Buttons_L | Buttons_R | Buttons_X | Buttons_Y | Buttons_Right | Buttons_Left | Buttons_Down | Buttons_Up | Buttons_A | Buttons_B))
		{
			PadReadOnP_(aiwk->pnum);
			aiwk->timer_rangeout = 0;
		}
		else
		{
			if (++aiwk->timer_rangeout > 300)
			{
				PadReadOffP_(aiwk->pnum);
				aiwk->timer_rangeout = 0;
				aiwk->action = AIActions::Run;
			}
		}

		NpcMilesManageRangeOut(aiwk, twp, target_twp);
		break;
	case AIActions::Wait:
		twp->Position = target_twp->Position;
		twp->Position.y += 1000000.0f;
		SetPlayerPosition_(aiwk->pnum, 0, &twp->Position, 0);
		if (NpcMilesCanBorn())
		{
			aiwk->action = AIActions::Start;
		}
		break;
	case AIActions::Unknown7:
		twp->Position = target_twp->Position;
		twp->Position.y += 1000000.0f;
		SetPlayerPosition_(aiwk->pnum, 0, &twp->Position, 0);
		aiwk->timer_rangeout += 1;

		if (aiwk->timer_rangeout > 3) {
			SetInputP_(twp, GeneralActions_ObjectCtrl);
			aiwk->action = AIActions::StandBy;
		}
		break;
	case AIActions::StandBy:
		twp->Position = target_twp->Position;
		twp->Position.y += 1000000.0f;
		SetPlayerPosition_(aiwk->pnum, 0, &twp->Position, 0);

		if (!NpcMilesStandByFlag)
		{
			ComeBackNpcMiles(aiwk, twp);
		}

		break;
	default:
		return;
	}

	// Todo: run 2p camera instead
	CameraScreensInfoArray[1] = CameraScreensInfoArray[0];
}

void __cdecl NpcMilesControlDestructor(ObjectMaster* obj)
{
	pNpcMilesTask = nullptr;
	EV_NpcMilesStandByOff();
}

void NpcMilesSet()
{
	if (!pNpcMilesTask /*&& NpcMilesCanBorn()*/)
	{
		pNpcMilesTask = LoadObject(0, "Miles2PControl", Miles2PControl, LoadObj_Data1);
		pNpcMilesTask->DeleteSub = NpcMilesControlDestructor;

		auto aiwk = (TailsAI*)pNpcMilesTask->Data1.Entity;
		aiwk->pnum = 1;
		aiwk->tgtnum = 0;

		EV_NpcMilesStandByOff();
		LoadTails(1);
	}
}

void __cdecl SetPlayer_r()
{
	TARGET_DYNAMIC(SetPlayer)();

	// Load Tails AI if in a Sonic level
	if (TwoPlayerMode == false)
	{
		if (NpcMilesCanBorn())
		{
			NpcMilesSet();
			WriteData<1>((char*)0x46B02E, (char)0x01); // Deathzone only for P1
		}
	}
	else
	{
		WriteData<1>((char*)0x46B02E, (char)0x02); // Restore DeathZone
	}
}

// Patches:

BOOL __cdecl DamagePlayer_r(EntityData1* data1, CharObj2Base* data2)
{
	if (pNpcMilesTask && data2->PlayerNum == 1 && data2->CharID == Characters_Tails)
	{
		return FALSE;
	}
	
	return DamagePlayer(data1, data2);
}

void __cdecl RemoveTailsVoice(int idk, int num)
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

void __cdecl RemoveTailsSound(int ID, int Entity, char Bank, char Volume)
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
		// Load Tails AI with Sonic
		SetPlayer_t = new Trampoline(0x43D630, 0x43D636, SetPlayer_r);

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