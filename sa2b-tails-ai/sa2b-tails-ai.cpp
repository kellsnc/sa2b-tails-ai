#include "stdafx.h"

bool IsTailsAI = false;

enum AIStates {
	AIState_Ground,
	AIState_Jumping,
	AIState_AirAction,
	AIState_Falling,
	AIState_Stop
};

void njCalcPoint(NJS_VECTOR *transform, NJS_VECTOR *out, float *matrix)
{
	out->x = matrix[1] * transform->y + *matrix * transform->x + matrix[2] * transform->z;
	out->y = matrix[4] * transform->x + matrix[5] * transform->y + matrix[6] * transform->z;;
	out->z = matrix[8] * transform->x + matrix[9] * transform->y + matrix[10] * transform->z;
}

void __fastcall njAddVector(NJS_VECTOR *vd, const NJS_VECTOR *vs)
{
	vd->x = vd->x + vs->x;
	vd->y = vs->y + vd->y;
	vd->z = vs->z + vd->z;
}

bool IsPointInsideSphere(NJS_VECTOR *center, NJS_VECTOR *pos, float radius) {
	return (powf(pos->x - center->x, 2) + pow(pos->y - center->y, 2) + pow(pos->z - center->z, 2)) <= pow(radius, 2);
}

bool IsPointInsideBox(NJS_VECTOR *p1, NJS_VECTOR *p2, NJS_VECTOR *pos) {
	return (pos->x > p1->x && pos->x < p2->x &&
		pos->y > p1->y && pos->y < p2->y &&
		pos->z > p1->z && pos->z < p2->z);
}

float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest) {
	return sqrtf(powf(dest->x - orig->x, 2) + powf(dest->y - orig->y, 2) + powf(dest->z - orig->z, 2));
}

Rotation PositionToRotation(NJS_VECTOR* orig, NJS_VECTOR* point) {
	NJS_VECTOR dist;
	Rotation result;

	dist.x = orig->x - point->x;
	dist.y = orig->y - point->y;
	dist.z = orig->z - point->z;

	result.x = atan2(dist.y, dist.z) * 65536.0 * -0.1591549762031479;
	result.y = atan2(dist.x, dist.z) * 65536.0 * 0.1591549762031479;

	result.y = -result.y - 0x4000;
	return result;
}

NJS_VECTOR AI_GetPointToFollow(NJS_VECTOR* pos, float rotation) {
	NJS_VECTOR point;
	NJS_VECTOR dir = { -10, 0, 0 };
	
	njPushMatrix(0);
	njTranslate(0, pos->x, pos->y, pos->z);
	njRotateY(_nj_current_matrix_ptr_, -rotation);
	njCalcPoint(&dir, &point, _nj_current_matrix_ptr_);
	njAddVector(&point, pos);
	return point;
}

int __cdecl GetPlayerNumberSP(ObjectMaster *a1) {
	if (IsTailsAI) {
		return 0;
	}
	else {
		EntityData1 *data = a1->Data1.Entity;
		if (data) {
			if (a1->Parent)
			{
				a1 = a1->Parent;
			}
			else if (data->field_2 == 5)
			{
				return data->NextAction;
			}

			char v2 = data->field_2;
			if (v2 && v2 <= 4u)
			{
				return a1->Data2.Character->PlayerNum;
			}
		}
	}

	return -1;
}

static void __declspec(naked) GetPlayerNumberSP_asm(ObjectMaster *a1)
{
	__asm
	{
		push eax
		call GetPlayerNumberSP
		add esp, 4 // a1<eax> is also used for return value
		retn
	}
}

void AI_Movement(EntityData1* data, EntityData1* playerdata, EntityData1* botdata, CharObj2Base* botco2, float dist, float distplayer) {
	//the Team Handler gives each AI a point which it must try to reach

	//collision box
	NJS_VECTOR p1 = { data->Scale.x - 5, data->Scale.y - 50, data->Scale.z - 5 };
	NJS_VECTOR p2 = { data->Scale.x + 5, data->Scale.y + 50, data->Scale.z + 5 };

	if (IsPointInsideBox(&p1, &p2, &botdata->Position)) {
		if (data->NextAction == AIState_AirAction) {
			botdata->Rotation.y = -PositionToRotation(&botdata->Position, &playerdata->Position).y;
			botco2->PhysData.Gravity = PhysicsArray[Characters_Tails].Gravity;
			botco2->AnimInfo.Current = 92;
			return;
		}

		data->field_2 = 0;
		botco2->Speed = { 0, 0, 0 };
	}
	else {
		//rotate player towards point to follow
		botdata->Rotation.y = PositionToRotation(&botdata->Position, &data->Scale).y;

		//calculate speed roughly
		botco2->Speed.x = dist / 10 < botco2->PhysData.HSpeedCap ? dist / 10 : botco2->PhysData.HSpeedCap;
	}
}

inline void AI_PerformJump(EntityData1* data, EntityData1* botdata, CharObj2Base* botco2, float height, float dist) {
	botco2->Speed.y = fmax(2, height / 5);
	botco2->Speed.x = fmax(1, dist / 20);
	data->NextAction = AIState_Jumping;
}


void AI_Obstacles(EntityData1* data, EntityData1* playerdata, EntityData1* botdata, CharObj2Base* botco2, float dist, float distplayer) {
	//check the conditions to jump, go around objects
	float height = botdata->Position.y - data->Scale.y;
	float test = PositionToRotation(&botdata->Position, &data->Scale).y;
	if (dist > 20 && dist < 40 && height < -8 && height > -20 &&
		test == botdata->Rotation.y && playerdata->Rotation.x == 0 && playerdata->Action < 5) {
		AI_PerformJump(data, botdata, botco2, height, dist);
	}

	if (dist > 200) {
		AI_PerformJump(data, botdata, botco2, height, dist);
	}

	if (FrameCountIngame % 100 == 0 && dist > 30) {
		if (IsPointInsideSphere(&data->Position, &botdata->Position, 5)) {
			AI_PerformJump(data, botdata, botco2, 10, 30);
			data->field_2 += 1;
		}

		if (data->field_2 > 3) {
			data->field_2 = 0;
			botdata->Position = data->Scale;
			botdata->Position.y += 50;
		}

		data->Position = botdata->Position;
	}

	if (dist > 600) {
		botdata->Position = data->Scale;
		botdata->Position.y += 50;
	}
}

void AI_InAir(EntityData1* data, EntityData1* playerdata, EntityData1* botdata, CharObj2Base* botco2, float dist, float distplayer) {
	if (data->NextAction == AIState_Jumping) {
		botco2->AnimInfo.Current = 66;
		botdata->Status |= Status_Ball;
		botdata->Status |= Status_Attack;
		botdata->Rotation.y = PositionToRotation(&botdata->Position, &data->Scale).y;

		if (dist > 200 && FrameCountIngame % 120 == 0) {
			data->NextAction = AIState_AirAction;
			botdata->Status &= ~Status_Attack;
			botdata->Status &= ~Status_Ball;
			botdata->Action = 15;
		}
	}
	else if (data->NextAction == AIState_AirAction) {
		botco2->PhysData.Gravity = 0.02;
		botco2->AnimInfo.Current = 90;
		
		AI_Movement(data, playerdata, botdata, botco2, dist, distplayer);
	}
}


void AI_Delete(ObjectMaster* obj) {
	IsTailsAI = false;
}

void AI_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1.Entity;
	EntityData1* playerdata = obj->Parent->Data1.Entity;
	EntityData1* botdata = MainCharObj1[1];
	CharObj2Base* botco2 = MainCharObj2[1];
	
	switch (data->Action) {
	case 0:
		LoadTails(1);
		data->Action = 1;
		return;
	case 1:
		botdata->Position = { 0, 0, 0 };
		if (obj->Parent->Data1.Entity->Status & Status_Ground) {
			botdata->Position = AI_GetPointToFollow(&playerdata->Position, playerdata->Rotation.y);
			botdata->Position.y += 30;
			data->Action = 2;
		}
		return;
	case 2:
		data->Scale = AI_GetPointToFollow(&playerdata->Position, playerdata->Rotation.y);
		
		float distplayer = GetDistance(&botdata->Position, &playerdata->Position);
		float dist = GetDistance(&botdata->Position, &data->Scale);

		if (botdata->Status & Status_Ground) {
			if (data->NextAction != AIState_Ground) {
				botdata->Status &= ~Status_Ball;
				botdata->Status &= ~Status_Attack;
				data->NextAction = AIState_Ground;
			}

			AI_Movement(data, playerdata, botdata, botco2, dist, distplayer);
			AI_Obstacles(data, playerdata, botdata, botco2, dist, distplayer);
		}
		else {
			AI_InAir(data, playerdata, botdata, botco2, dist, distplayer);
		}

		return;
	}
}

void LoadAI(ObjectMaster* parent) {
	ObjectMaster* AI = LoadObject(1, "AI", AI_Main, LoadObj_Data1);
	if (!IsTailsAI && parent == MainCharacter[0]) AI->DeleteSub = AI_Delete;
	AI->Parent = parent;
};

extern "C"
{
	__declspec(dllexport) void Init() {
		WriteJump((void*)0x46DCC0, GetPlayerNumberSP_asm);
	}

	__declspec(dllexport) void OnFrame() {
		if (GameState == GameStates_Ingame && CurrentCharacter == Characters_Sonic && !IsTailsAI && MainCharacter[0]) {
			LoadAI(MainCharacter[0]);
			IsTailsAI = true;
		}
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}