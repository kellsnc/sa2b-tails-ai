#include "stdafx.h"
#include "helper.h"
#include <cstdlib>

// Very hacky stuff to allow unique animations for Tails

static constexpr int animcount = 300;
static constexpr int bytecount = sizeof(AnimationIndex) * animcount;

static AnimationIndex CmnAnimations[animcount]{};
static AnimationIndex TailsAnimations[animcount]{};

extern ObjectMaster* pNpcMilesTask;

#pragma pack(push, 1)
struct AnimationInfo_
{
	__int16 AnimNum;
	__int16 ModelNum;
	__int16 mtnmode;
	__int16 NextAnimation;
	float TransitionSpeed;
	float AnimationSpeed;
};
#pragma pack(pop)


#pragma pack(push, 8)
struct CharAnimInfo_
{
	__int16 mtnmode;
	__int16 Next;
	__int16 Current;
	__int16 Animation3;
	__int16 field_8;
	__int16 acttimer;
	__int16 field_C;
	char field_E;
	char field_F;
	float nframe;
	float field_14;
	float field_18;
	char field_1C;
	char field_1D;
	char field_1E;
	char field_1F;
	char field_20;
	char field_21;
	char field_22;
	char field_23;
	AnimationInfo_* Animations;
	NJS_MOTION* Motion;
};
#pragma pack(pop)

void PlayAnimationThing_r(CharAnimInfo_* charAnimInfo);

void ResetCharacterAnim()
{
	for (int i = 86; i < 300; ++i)
	{
		CharacterAnimations[i].Index = i;
		CharacterAnimations[i].Count = 0;
		CharacterAnimations[i].Animation = 0;
	}
}

void CopyCharacterAnim()
{
	for (int i = 0; i < 300; ++i)
	{
		TailsAnimations[i].Index = CharacterAnimations[i].Index;
		TailsAnimations[i].Animation = CharacterAnimations[i].Animation;
		TailsAnimations[i].Count = CharacterAnimations[i].Count;
	}
}

AnimationIndex* LoadCmnMTN_(const char* name)
{
	auto mem = LoadMTNFile((char*)"PLCOMMTN.PRS");
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

AnimationIndex* MilesNewAnimList;

AnimationIndex* LoadTailsMTN_(const char* name)
{
	if (pNpcMilesTask)
	{
		MilesNewAnimList = LoadMTNFile((char*)name);
		CopyCharacterAnim();
		ResetCharacterAnim();
		return MilesNewAnimList;
	}
	else
	{
		return LoadMTNFile((char*)name);
	}
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

void __cdecl PSetTailsMotion_(CharAnimInfo* mjp)
{
	if (pNpcMilesTask)
	{
		PlayAnimationThing_r((CharAnimInfo_*)mjp);
	}
	else
	{
		PSetMotion(mjp);
	}
}

static void __declspec(naked) PSetTailsMotion()
{
	__asm
	{
		push esi
		call PSetTailsMotion_
		pop esi
		retn
	}
}


void* malloc_inline(size_t size)
{
	auto mem = (int*)MemoryManager__Allocate(size, nullptr, 0);
	*mem = (int)0x12345678;
	return (void*)((int)mem + 4);
}

//FastcallFunctionPointer(signed int, PInitializeInterpolateMotion2, (NJS_OBJECT* a1, CharAnimInfo_* a2), 0x469D20);
FunctionPointer(int, ListTheObjectPointerByTheEnd, (obj** objlist, int* pnum), 0x0446B30);

void __cdecl PInitializeInterpolateMotion2(CharAnimInfo_* anim)
{
	return;

	CharAnimInfo_* v1; // edi
	unsigned __int16 v2; // si
	int v3; // edx
	AnimationInfo_* plAction; // eax
	int v5; // edx
	obj* v6; // esi
	float* v7; // eax
	float* v8; // ecx
	unsigned int* v9; // ebp
	unsigned int v10; // edx
	unsigned int* v11; // ebx
	unsigned int v12; // esi
	unsigned int* v13; // ebx
	double v14; // st6
	unsigned int* v15; // eax
	int v16; // ebx
	double v17; // st7
	int v18; // ebx
	float* v19; // ebx
	double v20; // st7
	unsigned int v21; // edx
	int v22; // ebp
	unsigned int v23; // esi
	int v24; // ebp
	double v25; // st6
	int v26; // eax
	int v27; // ebx
	double v28; // st6
	float* v29; // eax
	float* v30; // esi
	unsigned int* v31; // ebp
	unsigned int v32; // edx
	unsigned int v33; // ecx
	unsigned int* v34; // ebx
	unsigned int* v35; // ebx
	double v36; // st6
	unsigned int* v37; // ebx
	int v38; // eax
	unsigned int* v39; // ebp
	unsigned int v40; // edx
	unsigned int v41; // edx
	unsigned int v42; // ecx
	int v43; // ebx
	int v44; // ebx
	double v45; // st6
	unsigned int* v46; // ebx
	int v47; // eax
	int v48; // eax
	int v49; // edx
	int v50; // ecx
	int v51; // ebx
	int v52; // ebp
	int v53; // edi
	int v54; // eax
	int v55; // eax
	int v56; // ecx
	float* v57; // esi
	int v58; // ecx
	int v59; // eax
	int v60; // ecx
	unsigned int* v61; // ebp
	unsigned int v62; // edx
	unsigned int* v63; // ebx
	unsigned int v64; // esi
	unsigned int* v65; // ebx
	double v66; // st6
	unsigned int* v67; // eax
	int v68; // ebx
	double v69; // st7
	int v70; // ebp
	unsigned int v71; // edx
	int v72; // ebx
	unsigned int v73; // esi
	int v74; // ebx
	double v75; // st6
	int v76; // eax
	int v77; // ebx
	unsigned int* v78; // [esp+Ch] [ebp-1F0h]
	int v79; // [esp+Ch] [ebp-1F0h]
	unsigned int* v80; // [esp+Ch] [ebp-1F0h]
	unsigned int* v81; // [esp+Ch] [ebp-1F0h]
	int v82; // [esp+Ch] [ebp-1F0h]
	unsigned int* v83; // [esp+Ch] [ebp-1F0h]
	int v84; // [esp+Ch] [ebp-1F0h]
	unsigned int v85; // [esp+10h] [ebp-1ECh]
	float v86; // [esp+10h] [ebp-1ECh]
	unsigned int v87; // [esp+10h] [ebp-1ECh]
	float v88; // [esp+10h] [ebp-1ECh]
	unsigned int v89; // [esp+10h] [ebp-1ECh]
	float v90; // [esp+10h] [ebp-1ECh]
	unsigned int v91; // [esp+10h] [ebp-1ECh]
	float v92; // [esp+10h] [ebp-1ECh]
	unsigned int v93; // [esp+10h] [ebp-1ECh]
	float v94; // [esp+10h] [ebp-1ECh]
	unsigned int v95; // [esp+10h] [ebp-1ECh]
	float v96; // [esp+10h] [ebp-1ECh]
	float* v97; // [esp+14h] [ebp-1E8h]
	unsigned int** v98; // [esp+18h] [ebp-1E4h]
	int* v99; // [esp+1Ch] [ebp-1E0h]
	obj* v100; // [esp+20h] [ebp-1DCh]
	unsigned int v101; // [esp+24h] [ebp-1D8h]
	unsigned int v102; // [esp+28h] [ebp-1D4h]
	unsigned int v103; // [esp+2Ch] [ebp-1D0h]
	NJS_MOTION* v104; // [esp+30h] [ebp-1CCh]
	NJS_MOTION* motion; // [esp+34h] [ebp-1C8h]
	int pnum; // [esp+38h] [ebp-1C4h] BYREF
	int v107; // [esp+3Ch] [ebp-1C0h]
	obj* objl[111]; // [esp+40h] [ebp-1BCh] BYREF


	v1 = anim;
	v2 = anim->Current;
	v3 = (unsigned __int16)anim->Next;
	objl[0] = CharacterModels[anim->Animations[v3].ModelNum].Model;
	pnum = 0;
	v107 = v3 * 16;
	ListTheObjectPointerByTheEnd(objl, &pnum);
	plAction = anim->Animations;
	motion = CharacterAnimations[*(__int16*)((char*)&plAction->AnimNum + v107)].Animation;
	v101 = (motion->type & 4 | 8u) >> 2;
	v104 = CharacterAnimations[plAction[v2].AnimNum].Animation;
	v99 = (int*)motion->mdata;
	v98 = (unsigned int**)v104->mdata;
	v97 = (float*)anim->Motion->mdata;
	v102 = (v104->type & 4 | 8u) >> 2;
	v103 = 0;
	if (CharacterAnimations[*(__int16*)((char*)&plAction->AnimNum + v107)].Count)
	{
		do
		{
			v6 = objl[v103];
			v100 = v6;
			if (!*v99 && !*v98
				|| (v7 = (float*)MemoryManager__Allocate(36, (char*)"..\\..\\src\\player.c", 11606),
					v8 = v7 + 1,
					*v7 = 5.6904566e-28,
					v7 == (float*)-4))
			{
				*v97 = 0;
				v97[2] = 0;
				v19 = v97;
				goto LABEL_36;
			}
			*v97 = *v8;
			*v8 = 0.0f;
			v9 = *v98;
			if (*v98)
			{
				v11 = v98[2];
				v12 = 0;
				v10 = 0;
				if ((double)*v9 < v1->nframe)
				{
					v12 = (unsigned int)v11 - 1;
					v85 = (unsigned int)v11 - 1;
					v13 = *v98;
					while (v10 < v12)
					{
						v14 = (double)v13[4];
						v13 += 4;
						v12 = v10++;
						if (v14 >= v1->nframe)
						{
							goto LABEL_13;
						}
						v12 = v85;
					}
					v10 = 0;
				}
			LABEL_13:
				v15 = &v9[4 * v12];
				*(float*)(v8 + 4) = *((float*)v15 + 1);
				*(float*)(v8 + 8) = *((float*)v15 + 2);
				*(float*)(v8 + 12) = *((float*)v15 + 3);
				if (v10 == v12)
				{
					goto LABEL_19;
				}
				v78 = &v9[4 * v10];
				v16 = *v78 - *v15;
				if (v10 < v12)
				{
					v16 += v104->nbFrame;
				}
				v86 = (v1->nframe - (double)*v15) / (double)(unsigned int)v16;
				*(float*)(v8 + 4) = (*((float*)v78 + 1) - *((float*)v15 + 1)) * v86 + *(float*)(v8 + 4);
				*(float*)(v8 + 8) = (*((float*)v78 + 2) - *((float*)v15 + 2)) * v86 + *(float*)(v8 + 8);
				v17 = v86 * (*((float*)v78 + 3) - *((float*)v15 + 3)) + *(float*)(v8 + 12);
			}
			else
			{
				*(float*)(v7 + 8) = v6->pos[0];
				*(float*)(v7 + 12) = v6->pos[1];
				v17 = v6->pos[2];
			}
			*(float*)(v8 + 12) = v17;
		LABEL_19:
			*(DWORD*)(v8 + 16) = 1;
			v18 = *v99;
			if (!*v99)
			{
				*(float*)(v8 + 20) = v100->pos[0];
				*(float*)(v8 + 24) = v100->pos[1];
				*(float*)(v8 + 28) = v100->pos[2];
			LABEL_34:
				v19 = v97;
				v97[2] = 2;
				goto LABEL_36;
			}
			if (0.0 == v1->field_18)
			{
				*(float*)(v8 + 20) = *(float*)(v18 + 4);
				*(float*)(v8 + 24) = *(float*)(v18 + 8);
				v20 = *(float*)(v18 + 12);
				v19 = v97;
				*(float*)(v8 + 28) = v20;
				v97[2] = 2;
				goto LABEL_36;
			}
			v22 = v99[2];
			v23 = 0;
			v21 = 0;
			if ((double)*(unsigned int*)v18 < v1->field_18)
			{
				v23 = v22 - 1;
				v87 = v22 - 1;
				v24 = *v99;
				while (v21 < v23)
				{
					v25 = (double)*(unsigned int*)(v24 + 16);
					v24 += 16;
					v23 = v21++;
					if (v25 >= v1->field_18)
					{
						goto LABEL_29;
					}
					v23 = v87;
				}
				v21 = 0;
			}
		LABEL_29:
			v26 = v18 + 16 * v23;
			*(float*)(v8 + 20) = *(float*)(v26 + 4);
			*(float*)(v8 + 24) = *(float*)(v26 + 8);
			*(float*)(v8 + 28) = *(float*)(v26 + 12);
			if (v21 == v23)
			{
				goto LABEL_34;
			}
			v79 = 16 * v21 + v18;
			v27 = *(DWORD*)v79 - *(DWORD*)v26;
			if (v21 < v23)
			{
				v27 += motion->nbFrame;
			}
			v28 = (double)(unsigned int)v27;
			v19 = v97;
			v88 = (v1->field_18 - (double)*(unsigned int*)v26) / v28;
			*(float*)(v8 + 20) = (*(float*)(v79 + 4) - *(float*)(v26 + 4)) * v88 + *(float*)(v8 + 20);
			*(float*)(v8 + 24) = (*(float*)(v79 + 8) - *(float*)(v26 + 8)) * v88 + *(float*)(v8 + 24);
			*(float*)(v8 + 28) = v88 * (*(float*)(v79 + 12) - *(float*)(v26 + 12)) + *(float*)(v8 + 28);
			v97[2] = 2;
		LABEL_36:
			if (!v99[1] && !v98[1]
				|| (v29 = (float*)MemoryManager__Allocate(36, (char*)"..\\..\\src\\player.c", 11703),
					v30 = v29 + 1,
					// *v29 = 305419896,
					v29 == (float*)-4))
			{
				v57 = v97;
				v19[1] = 0;
				v19[3] = 0;
				goto LABEL_91;
			}
			v97[1] = *v30;
			*v30 = 0;
			v31 = v98[1];
			if (v31)
			{
				v34 = v98[3];
				v32 = 0;
				v33 = 0;
				if ((double)*v31 < v1->nframe)
				{
					v32 = (unsigned int)v34 - 1;
					v89 = (unsigned int)v34 - 1;
					v35 = v98[1];
					while (v33 < v32)
					{
						v36 = (double)v35[4];
						v35 += 4;
						v32 = v33++;
						if (v36 >= v1->nframe)
						{
							goto LABEL_47;
						}
						v32 = v89;
					}
					v33 = 0;
				}
			LABEL_47:
				v37 = &v31[4 * v32];
				v29[2] = v37[1];
				v29[3] = v37[2];
				v29[4] = v37[3];
				if (v33 != v32)
				{
					v80 = &v31[4 * v33];
					v38 = *v80 - *v37;
					if (v33 < v32)
					{
						v38 += v104->nbFrame;
					}
					v90 = (v1->nframe - (double)*v37) / (double)(unsigned int)v38;
					v30[1] += (int)((double)(__int16)(*((WORD*)v80 + 2) - *((WORD*)v37 + 2)) * v90);
					v30[2] += (int)((double)(__int16)(*((WORD*)v80 + 4) - *((WORD*)v37 + 4)) * v90);
					v30[3] += (int)(v90 * (double)(__int16)(*((WORD*)v80 + 6) - *((WORD*)v37 + 6)));
				}
			}
			else
			{
				v29[2] = v100->ang[0];
				v29[3] = v100->ang[1];
				v29[4] = v100->ang[2];
			}
			v30[4] = 1;
			v39 = (unsigned int*)v99[1];
			if (!v39)
			{
				v30[5] = v100->ang[0];
				v30[6] = v100->ang[1];
				v40 = v100->ang[2];
				goto LABEL_67;
			}
			if (0.0 == v1->field_18)
			{
				v30[5] = v39[1];
				v30[6] = v39[2];
				v40 = v39[3];
			LABEL_67:
				v30[7] = v40;
				goto LABEL_68;
			}
			v43 = v99[3];
			v41 = 0;
			v42 = 0;
			if ((double)*v39 < v1->field_18)
			{
				v41 = v43 - 1;
				v91 = v43 - 1;
				v44 = v99[1];
				while (v42 < v41)
				{
					v45 = (double)*(unsigned int*)(v44 + 16);
					v44 += 16;
					v41 = v42++;
					if (v45 >= v1->field_18)
					{
						goto LABEL_62;
					}
					v41 = v91;
				}
				v42 = 0;
			}
		LABEL_62:
			v46 = &v39[4 * v41];
			v30[5] = v46[1];
			v30[6] = v46[2];
			v30[7] = v46[3];
			if (v42 != v41)
			{
				v81 = &v39[4 * v42];
				v47 = *v81 - *v46;
				if (v42 < v41)
				{
					v47 += motion->nbFrame;
				}
				v92 = (v1->field_18 - (double)*v46) / (double)(unsigned int)v47;
				v30[5] += (int)((double)(__int16)(*((WORD*)v81 + 2) - *((WORD*)v46 + 2)) * v92);
				v30[6] += (int)((double)(__int16)(*((WORD*)v81 + 4) - *((WORD*)v46 + 4)) * v92);
				v30[7] += (int)(v92 * (double)(__int16)(*((WORD*)v81 + 6) - *((WORD*)v46 + 6)));
			}
		LABEL_68:
			v50 = *((__int16*)v30 + 10);
			v48 = *((__int16*)v30 + 2);
			v49 = *((__int16*)v30 + 4);
			v51 = *((__int16*)v30 + 6);
			v53 = *((__int16*)v30 + 12);
			v82 = *((__int16*)v30 + 14);
			v30[7] = v82;
			v52 = (__int16)(v50 - v48);
			v30[1] = v48;
			v30[2] = v49;
			v30[3] = v51;
			v30[5] = v50;
			v30[6] = v53;
			if (v52 > 0)
			{
				if (v48 <= v50)
				{
					goto LABEL_72;
				}
				v48 = v50 + ((((unsigned int)(v48 - v50 - 1) >> 16) + 1) << 16);
			LABEL_71:
				v50 = v48;
				goto LABEL_72;
			}
			if (v52 >= 0)
			{
				goto LABEL_71;
			}
			if (v48 < v50)
			{
				v50 -= (((unsigned int)(v50 - v48 - 1) >> 16) + 1) << 16;
			}
		LABEL_72:
			v54 = (__int16)(v53 - v49);
			v30[5] = v50;
			if (v54 > 0)
			{
				if (v49 <= v53)
				{
					goto LABEL_76;
				}
				v49 = v53 + ((((unsigned int)(v49 - v53 - 1) >> 16) + 1) << 16);
			LABEL_75:
				v53 = v49;
				goto LABEL_76;
			}
			if (v54 >= 0)
			{
				goto LABEL_75;
			}
			if (v49 < v53)
			{
				v53 -= (((unsigned int)(v53 - v49 - 1) >> 16) + 1) << 16;
			}
		LABEL_76:
			v55 = v82;
			v56 = (__int16)(v82 - v51);
			v30[6] = v53;
			if (v56 <= 0)
			{
				if (v56 < 0)
				{
					if (v51 < v82)
					{
						v55 = v82 - ((((unsigned int)(v82 - v51 - 1) >> 16) + 1) << 16);
					}
					goto LABEL_80;
				}
				goto LABEL_79;
			}
			if (v51 > v82)
			{
				v51 = v82 + ((((unsigned int)(v51 - v82 - 1) >> 16) + 1) << 16);
			LABEL_79:
				v55 = v51;
			}
		LABEL_80:
			v1 = anim;
			v30[7] = v55;
			v57 = v97;
			v97[3] = 2;
		LABEL_91:
			if ((WORD)v102 != 3 && (WORD)v101 != 3)
			{
				goto LABEL_132;
			}
			v58 = v57[2];
			v57[4] = v57[3];
			v57[3] = v58;
			if (((WORD)v101 != 3 || !v99[2]) && ((WORD)v102 != 3 || !v98[2])
				|| (v59 = (int)MemoryManager__Allocate(36, (char*)"..\\..\\src\\player.c", 11830),
					v60 = v59 + 4,
					//*(DWORD*)v59 = 305419896,
					v59 == -4))
			{
				v57[2] = 0;
				v57[5] = 0;
				goto LABEL_131;
			}
			v57[2] = v60;
			*(DWORD*)v60 = 0;
			if ((WORD)v102 == 3 && (v61 = v98[2]) != 0)
			{
				v63 = v98[5];
				v64 = 0;
				v62 = 0;
				if ((double)*v61 < v1->nframe)
				{
					v64 = (unsigned int)v63 - 1;
					v93 = (unsigned int)v63 - 1;
					v65 = v98[2];
					while (v62 < v64)
					{
						v66 = (double)v65[4];
						v65 += 4;
						v64 = v62++;
						if (v66 >= v1->nframe)
						{
							goto LABEL_107;
						}
						v64 = v93;
					}
					v62 = 0;
				}
			LABEL_107:
				v67 = &v61[4 * v64];
				*(float*)(v60 + 4) = *((float*)v67 + 1);
				*(float*)(v60 + 8) = *((float*)v67 + 2);
				*(float*)(v60 + 12) = *((float*)v67 + 3);
				if (v62 == v64)
				{
					goto LABEL_113;
				}
				v83 = &v61[4 * v62];
				v68 = *v83 - *v67;
				if (v62 < v64)
				{
					v68 += v104->nbFrame;
				}
				v94 = (v1->nframe - (double)*v67) / (double)(unsigned int)v68;
				*(float*)(v60 + 4) = (*((float*)v83 + 1) - *((float*)v67 + 1)) * v94 + *(float*)(v60 + 4);
				*(float*)(v60 + 8) = (*((float*)v83 + 2) - *((float*)v67 + 2)) * v94 + *(float*)(v60 + 8);
				v69 = v94 * (*((float*)v83 + 3) - *((float*)v67 + 3)) + *(float*)(v60 + 12);
			}
			else
			{
				*(float*)(v59 + 8) = v100->scl[0];
				*(float*)(v59 + 12) = v100->scl[1];
				v69 = v100->scl[2];
			}
			*(float*)(v60 + 12) = v69;
		LABEL_113:
			*(DWORD*)(v60 + 16) = 1;
			if ((WORD)v101 != 3 || (v70 = v99[2]) == 0)
			{
				*(float*)(v60 + 20) = v100->scl[0];
				*(float*)(v60 + 24) = v100->scl[1];
				*(float*)(v60 + 28) = v100->scl[2];
			LABEL_129:
				v97[5] = 2;
				goto LABEL_131;
			}
			if (0.0 == v1->field_18)
			{
				*(float*)(v60 + 20) = *(float*)(v70 + 4);
				*(float*)(v60 + 24) = *(float*)(v70 + 8);
				*(float*)(v60 + 28) = *(float*)(v70 + 12);
				v97[5] = 2;
				goto LABEL_131;
			}
			v72 = v99[5];
			v73 = 0;
			v71 = 0;
			if ((double)*(unsigned int*)v70 < v1->field_18)
			{
				v73 = v72 - 1;
				v95 = v72 - 1;
				v74 = v99[2];
				while (v71 < v73)
				{
					v75 = (double)*(unsigned int*)(v74 + 16);
					v74 += 16;
					v73 = v71++;
					if (v75 >= v1->field_18)
					{
						goto LABEL_124;
					}
					v73 = v95;
				}
				v71 = 0;
			}
		LABEL_124:
			v76 = v70 + 16 * v73;
			*(float*)(v60 + 20) = *(float*)(v76 + 4);
			*(float*)(v60 + 24) = *(float*)(v76 + 8);
			*(float*)(v60 + 28) = *(float*)(v76 + 12);
			if (v71 == v73)
			{
				goto LABEL_129;
			}
			v84 = v70 + 16 * v71;
			v77 = *(DWORD*)v84 - *(DWORD*)v76;
			if (v71 < v73)
			{
				v77 += motion->nbFrame;
			}
			v96 = (v1->field_18 - (double)*(unsigned int*)v76) / (double)(unsigned int)v77;
			*(float*)(v60 + 20) = (*(float*)(v84 + 4) - *(float*)(v76 + 4)) * v96 + *(float*)(v60 + 20);
			*(float*)(v60 + 24) = (*(float*)(v84 + 8) - *(float*)(v76 + 8)) * v96 + *(float*)(v60 + 24);
			*(float*)(v60 + 28) = v96 * (*(float*)(v84 + 12) - *(float*)(v76 + 12)) + *(float*)(v60 + 28);
			v97[5] = 2;
		LABEL_131:
			if ((WORD)v101 == 3)
			{
				goto LABEL_134;
			}
		LABEL_132:
			if ((WORD)v102 == 3)
			{
			LABEL_134:
				v97 += 6;
				goto LABEL_135;
			}
			v97 += 4;
		LABEL_135:
			if ((WORD)v101 == 3)
			{
				v99 += 6;
			}
			else
			{
				v99 += 4;
			}
			if ((WORD)v102 == 3)
			{
				v98 += 6;
			}
			else
			{
				v98 += 4;
			}
			++v103;
		} while (v103 < (__int16)CharacterAnimations[*(__int16*)((char*)&v1->Animations->AnimNum + v107)].Count);
	}
	if ((WORD)v101 == 3 || (WORD)v102 == 3)
	{
		v1->Motion->type = 7;
		v1->Motion->inp_fn = 3;
	}
	else
	{
		v1->Motion->type = 3;
		v1->Motion->inp_fn = 2;
	}
}


void PSetMotion_SetStartFrame(CharAnimInfo_* result)
{
	__int16 mtnmode;
	Uint32 v2;

	mtnmode = result->mtnmode;
	if (result->mtnmode == 7 || mtnmode == 8)
	{
		v2 = TailsAnimations[result->Animations[result->Current].AnimNum].Animation->nbFrame;
		result->acttimer = 0;
		result->nframe = (double)v2 - 1.0;
	}
	else if (mtnmode == 3 || mtnmode == 11 || mtnmode == 10 || mtnmode == 13)
	{
		result->nframe = result->field_18;
		result->acttimer = 0;
		result->field_18 = 0.0;
	}
	else
	{
		result->nframe = 0.0;
		result->acttimer = 0;
	}
}

void PTerminateInterpolateMotion(CharAnimInfo_* a1)
{
	NJS_MOTION* motion; // ecx
	int i; // ebp
	Uint16 inp_fn; // di
	void* mdata; // esi
	AllocatedMem* v6; // eax
	int v7; // eax
	DWORD* v8; // esi
	AllocatedMem* v10; // eax
	DWORD* v11; // esi
	MemManFuncs* memman; // ecx
	AllocatedMem* v13; // eax

	motion = a1->Motion;
	mdata = motion->mdata;
	inp_fn = motion->inp_fn;
	for (i = (__int16)TailsAnimations[a1->Animations[a1->Next].AnimNum].Count; i > 0; --i)
	{
		if (*(DWORD*)mdata)
		{
			v6 = (AllocatedMem*)(*(DWORD*)mdata - 4);
			// v6->Cookie = 0x89ABCDEF;
			MemoryManager__Deallocate(v6, (char*)"..\\..\\src\\player.c", 11988);
			*(DWORD*)mdata = 0;
			*((DWORD*)mdata + inp_fn) = 0;
		}
		v7 = *((DWORD*)mdata + 1);
		v8 = (DWORD*)mdata + 4;
		if (v7)
		{
			v10 = (AllocatedMem*)(v7 - 4);
			//v10->Cookie = 0x89ABCDEF;
			MemoryManager__Deallocate(v10, (char*)"..\\..\\src\\player.c", 11996);
			*v8 = 0;
			v8[inp_fn] = 0;
		}
		v11 = v8 + 1;
		if (inp_fn == 3)
		{
			if (*v11)
			{
				memman = MemoryManager;
				v13 = (AllocatedMem*)(*v11 - 4);
				// v13->Cookie = 0x89ABCDEF;
				memman->Deallocate(v13, (char*)"..\\..\\src\\player.c", 12004);
				*v11 = 0;
				v11[3] = 0;
			}
			mdata = v11 + 4;
		}
		else
		{
			mdata = v11 + 2;
		}
	}
}

void PlayAnimationThing_r(CharAnimInfo_* charAnimInfo)
{
	__int16 NextAnim; // dx
	unsigned __int16 nextAnimCopy; // bx
	__int16 v3; // ax
	int curAnim; // edi
	__int16 v5; // ax
	AnimationInfo_* v6; // eax
	AnimationInfo_* v7; // edx
	AnimationInfo_* v8; // eax
	double v9; // st7
	double v10; // st7
	MemManFuncs* v11; // ecx
	NJS_MOTION* motion; // eax
	AnimationInfo_* charAnimCopy; // edx
	AnimationInfo_* v14; // ecx
	int v15; // ecx
	BOOL v16; // c3
	NJS_OBJECT* model; // ecx
	AnimationInfo_* v18; // ecx
	AnimationInfo_* v19; // eax
	float v20; // ecx
	float v21; // st6
	int v22; // ecx
	double v23; // st7
	AnimationInfo_* v24; // eax
	float v25; // ecx
	double v26; // st7
	int v27; // ecx
	float v28; // st7
	float v29; // ecx
	double v30; // st7
	AnimationInfo_* v31; // eax
	BOOL v32; // pf
	NJS_MOTION* v33; // eax
	CharAnimInfo_* v34; // eax
	unsigned __int16 v35; // dx
	AnimationInfo_* v36; // eax
	AnimationInfo_* v37; // ecx
	float v38; // st6
	int v39; // ecx
	int v40; // edx
	float v41; // st7
	NJS_MOTION* v42; // eax
	AnimationInfo_* v43; // edx
	AnimationInfo_* v44; // ecx
	int v45; // ecx
	float v46; // st7
	NJS_MOTION* v47; // eax
	AnimationInfo_* v48; // edx
	__int16 v49;
	double v50;
	NJS_OBJECT* v51;
	__int16 v52;
	AnimationInfo_* v53;
	AnimationInfo_* v54;
	int v55;
	float v56;
	NJS_MOTION* v57;
	AnimationInfo_* v58;
	__int16 v59;
	double v60;
	NJS_OBJECT* v61;
	AnimationInfo_* v62;
	__int16 v63;
	double v64;
	AnimationInfo_* v65;
	NJS_MOTION* v66;
	float v67;
	double v68;
	NJS_MOTION* v69;
	AllocatedMem* v70;
	NJS_MOTION* v71;
	AllocatedMem* v72;
	AnimationInfo_* v73;
	__int16 v74;
	int v75;
	float v76;
	float v77;
	float v78;
	float v79;
	float v80;
	float v81;
	float v82;
	float v83;
	float v84;
	float v85;
	float v86;
	float v87;
	float v88;
	float v89;
	float v90;
	float v91;
	float v92;
	float v93;
	float v94;
	float v95;
	float v96;
	float v97;

	NextAnim = charAnimInfo->Next;
	v75 = 0;
	nextAnimCopy = NextAnim;
	if ((charAnimInfo->field_C & 4) != 0)
	{
		v75 = 1;
	}
	v3 = charAnimInfo->Current;
	charAnimInfo->field_14 = charAnimInfo->nframe;
	charAnimInfo->field_C = 0;
	curAnim = v3;
	charAnimInfo->Animation3 = v3;
	if (v3 == (unsigned __int16)NextAnim || (v5 = charAnimInfo->mtnmode, charAnimInfo->mtnmode == 2) || !v5 || v5 == 15)
	{
		++charAnimInfo->acttimer;
	}
	else
	{
		charAnimInfo->acttimer = 0;
		v6 = charAnimInfo->Animations;
		v7 = &v6[NextAnim];
		charAnimInfo->mtnmode = 1;
		if (v7->mtnmode == 11)
		{
			v8 = &v6[curAnim];
			if (v8->mtnmode == 11)
			{
				v91 = (float)TailsAnimations[v8->AnimNum].Animation->nbFrame;
				v93 = (float)TailsAnimations[v7->AnimNum].Animation->nbFrame;
				v9 = v93;
				v92 = (v91 - charAnimInfo->nframe) * v93 / v91 + 0.1000000014901161 * v93;
				charAnimInfo->field_18 = v92;
				if (v92 >= v9)
				{
					charAnimInfo->field_18 = v92 - v9;
				}
			}
		}
	}
	v10 = 1.0;
	switch (charAnimInfo->mtnmode)
	{
	case 0:
		v11 = MemoryManager;
		charAnimInfo->Current = nextAnimCopy;
		charAnimInfo->mtnmode = 1;
		charAnimInfo->field_18 = 0.0;
		charAnimInfo->nframe = 0.0;
		motion = (NJS_MOTION*)malloc_inline(16); //(NJS_MOTION*)MemoryManager__Allocate(16, (char*)"..\\..\\src\\player.c", 11209);
		charAnimInfo->Motion = motion;
		motion->nbFrame = 2;
		charAnimInfo->Motion->type = 3;
		charAnimInfo->Motion->inp_fn = 2;
		charAnimInfo->Motion->mdata = AllocateArray(24, 111, (char*)"..\\..\\src\\player.c", 11216);
		return;
	case 1:
		if (v75
			|| (charAnimCopy = charAnimInfo->Animations,
				v15 = nextAnimCopy,
				v16 = 1.0 == charAnimCopy[v15].TransitionSpeed,
				v14 = &charAnimCopy[v15],
				v16)
			|| (model = CharacterModels[v14->ModelNum].Model,
				model != CharacterModels[charAnimCopy[curAnim].ModelNum].Model))
		{
			charAnimInfo->nframe = 1.0;
		}
		else
		{
			PInitializeInterpolateMotion2(charAnimInfo);
			charAnimInfo->nframe = 0.0;
			v10 = 1.0;
		}
		charAnimInfo->Current = nextAnimCopy;
		charAnimInfo->mtnmode = 2;
		goto LABEL_20;
	case 2:
	LABEL_20:
		v18 = &charAnimInfo->Animations[charAnimInfo->Current];
		v94 = v18->TransitionSpeed + charAnimInfo->nframe;
		charAnimInfo->nframe = v94;
		if (v94 >= v10)
		{
			goto LABEL_21;
		}
		return;
	case 3:
		v19 = charAnimInfo->Animations;
		v22 = nextAnimCopy;
		v23 = v19[v22].AnimationSpeed;

		v20 = TailsAnimations[v19[v22].AnimNum].Animation->nbFrame;
		v76 = v23 + charAnimInfo->nframe;
		charAnimInfo->nframe = v76;
		v21 = v20 - 1.0f;
		if (v76 >= v21)
		{
			do
			{
				charAnimInfo->nframe = charAnimInfo->nframe - v21;
				v21 = v20 - 1.0;
			} while (charAnimInfo->nframe >= v21);
		}
		for (; charAnimInfo->nframe < 0.0; charAnimInfo->nframe = v20 + charAnimInfo->nframe)
		{
			;
		}
		return;
	case 4:
		v36 = charAnimInfo->Animations;
		v39 = nextAnimCopy;
		v40 = v36[v39].AnimNum;
		v41 = v36[v39].AnimationSpeed + charAnimInfo->nframe;
		v37 = &v36[v39];
		v42 = TailsAnimations[v40].Animation;
		v81 = v41;
		charAnimInfo->nframe = v81;
		v38 = (double)v42->nbFrame - 1.0;
		if (v38 > v81)
		{
			if (v38 <= v81 + v37->AnimationSpeed)
			{
				charAnimInfo->field_C = 2;
			}
		}
		else
		{
			v35 = nextAnimCopy;
			v34 = charAnimInfo;
		LABEL_74:
			v73 = v34->Animations;
			v74 = LOBYTE(v73[v35].NextAnimation);
			v34->Next = v74;
			v34->Current = v74;
			v34->mtnmode = v73[v74].mtnmode;
			v73->NextAnimation = LOBYTE(v73[v34->Next].NextAnimation);
			v34->field_C |= 1u;
			v34->field_8 = (__int16)v73->NextAnimation;
			PSetMotion_SetStartFrame(v34);
		}
		return;
	case 5:
		v43 = charAnimInfo->Animations;
		v45 = nextAnimCopy;
		v46 = v43[v45].AnimationSpeed + charAnimInfo->nframe;
		v47 = TailsAnimations[v43[v45].AnimNum].Animation;
		v44 = &v43[v45];
		v82 = v46;
		charAnimInfo->nframe = v82;
		if ((double)v47->nbFrame - 1.0 <= v82)
		{
			v50 = 1.0;
			charAnimInfo->field_8 = v44->NextAnimation;
			v49 = charAnimInfo->field_8;
			charAnimInfo->acttimer = 0;
			v48 = &v43[v49];
			if (1.0 == v48->TransitionSpeed
				|| (v51 = CharacterModels[v44->ModelNum].Model, v51 != CharacterModels[v48->ModelNum].Model))
			{
				charAnimInfo->nframe = 1.0;
			}
			else
			{
				nextAnimCopy = v49;
				charAnimInfo->Next = (unsigned __int8)v49;
				PInitializeInterpolateMotion2(charAnimInfo);
				charAnimInfo->nframe = 0.0;
				v50 = 1.0;
			}
			v18 = &charAnimInfo->Animations[nextAnimCopy];
			charAnimInfo->Current = nextAnimCopy;
			v52 = LOBYTE(v18->NextAnimation);
			charAnimInfo->mtnmode = 2;
			charAnimInfo->field_8 = v52;
			v83 = v18->TransitionSpeed + charAnimInfo->nframe;
			charAnimInfo->nframe = v83;
			if (v83 >= v50)
			{
			LABEL_21:
				charAnimInfo->mtnmode = v18->mtnmode;
				PTerminateInterpolateMotion(charAnimInfo);
				PSetMotion_SetStartFrame(charAnimInfo);
			}
		}
		return;
	case 6:
		v65 = &charAnimInfo->Animations[nextAnimCopy];
		v66 = TailsAnimations[v65->AnimNum].Animation;
		v87 = v65->AnimationSpeed + charAnimInfo->nframe;
		charAnimInfo->nframe = v87;
		v64 = (double)v66->nbFrame - 1.0;
		if (v64 <= v87)
		{
			charAnimInfo->nframe = v64;
			charAnimInfo->field_C = 1;
		}
		return;
	case 7:
		v84 = charAnimInfo->nframe - charAnimInfo->Animations[nextAnimCopy].AnimationSpeed;
		charAnimInfo->nframe = v84;
		v32 = v84 >= 0.0;
		goto LABEL_38;
	case 8:
		v88 = charAnimInfo->nframe - charAnimInfo->Animations[nextAnimCopy].AnimationSpeed;
		charAnimInfo->nframe = v88;
		if (v88 < 0.0)
		{
			charAnimInfo->nframe = 0.0;
			charAnimInfo->field_C = 1;
		}
		return;
	case 9:
		v53 = charAnimInfo->Animations;
		v55 = nextAnimCopy;
		v56 = v53[v55].AnimationSpeed + charAnimInfo->nframe;
		v57 = TailsAnimations[v53[v55].AnimNum].Animation;
		v54 = &v53[v55];
		v85 = v56;
		charAnimInfo->nframe = v85;
		if ((double)v57->nbFrame - 1.0 <= v85)
		{
			v59 = charAnimInfo->field_8;
			v60 = 1.0;
			charAnimInfo->acttimer = 0;
			v58 = &v53[v59];
			if (1.0 == v58->TransitionSpeed
				|| (v61 = CharacterModels[v54->ModelNum].Model, v61 != CharacterModels[v58->ModelNum].Model))
			{
				charAnimInfo->nframe = 1.0;
			}
			else
			{
				nextAnimCopy = v59;
				charAnimInfo->Next = (unsigned __int8)v59;
				PInitializeInterpolateMotion2(charAnimInfo);
				charAnimInfo->nframe = 0.0;
				v60 = 1.0;
			}
			v62 = &charAnimInfo->Animations[nextAnimCopy];
			charAnimInfo->Current = nextAnimCopy;
			v63 = LOBYTE(v62->NextAnimation);
			charAnimInfo->mtnmode = 2;
			charAnimInfo->field_8 = v63;
			v86 = v62->TransitionSpeed + charAnimInfo->nframe;
			charAnimInfo->nframe = v86;
			if (v86 >= v60)
			{
				charAnimInfo->mtnmode = v62->mtnmode;
				PTerminateInterpolateMotion(charAnimInfo);
				PSetMotion_SetStartFrame(charAnimInfo);
			}
			charAnimInfo->field_C |= 1u;
		}
		return;
	case 10:
		v24 = charAnimInfo->Animations;
		v27 = nextAnimCopy;
		v28 = **(float**)&charAnimInfo->field_1C * v24[v27].AnimationSpeed;
		v25 = TailsAnimations[v24[v27].AnimNum].Animation->nbFrame;
		v95 = v28 + charAnimInfo->nframe;
		charAnimInfo->nframe = v95;
		v77 = v25;
		v26 = v77;
		if (v77 <= (double)v95)
		{
			do
			{
				charAnimInfo->nframe = charAnimInfo->nframe - v26;
				v78 = v25;
				v26 = v78;
			} while (charAnimInfo->nframe >= (double)v78);
		}
		for (; charAnimInfo->nframe < 0.0; charAnimInfo->nframe = v25 + charAnimInfo->nframe)
		{
			;
		}
		return;
	case 11:
		v31 = &charAnimInfo->Animations[nextAnimCopy];
		v29 = TailsAnimations[v31->AnimNum].Animation->nbFrame;
		v96 = **(float**)&charAnimInfo->field_20 * v31->AnimationSpeed + charAnimInfo->nframe;
		charAnimInfo->nframe = v96;
		v79 = v29;
		v30 = v79;
		if (v79 <= (double)v96)
		{
			do
			{
				charAnimInfo->nframe = charAnimInfo->nframe - v30;
				v80 = v29;
				v30 = v80;
			} while (charAnimInfo->nframe >= (double)v80);
		}
		for (; charAnimInfo->nframe < 0.0; charAnimInfo->nframe = v29 + charAnimInfo->nframe)
		{
			;
		}
		return;
	case 12:
		v33 = TailsAnimations[charAnimInfo->Animations[nextAnimCopy].AnimNum].Animation;
		v97 = **(float**)&charAnimInfo->field_20 + charAnimInfo->nframe;
		charAnimInfo->nframe = v97;
		v32 = (double)v33->nbFrame - 1.0 > v97;
	LABEL_38:
		if (v32)
		{
			return;
		}
		v35 = nextAnimCopy;
		v34 = charAnimInfo;
		goto LABEL_74;
	case 13:
		v67 = TailsAnimations[charAnimInfo->Animations[nextAnimCopy].AnimNum].Animation->nbFrame;
		v89 = v67;
		v68 = v89;
		if (charAnimInfo->nframe >= (double)v89)
		{
			do
			{
				charAnimInfo->nframe = charAnimInfo->nframe - v68;
				v90 = v67;
				v68 = v90;
			} while (charAnimInfo->nframe >= (double)v90);
		}
		for (; charAnimInfo->nframe < 0.0; charAnimInfo->nframe = v67 + charAnimInfo->nframe)
		{
			;
		}
		return;
	default:
		PTerminateInterpolateMotion(charAnimInfo);
		v69 = charAnimInfo->Motion;
		if (v69)
		{
			if (v69->mdata)
			{
				v70 = (AllocatedMem*)((char*)v69->mdata - 4);
				MemoryManager__Deallocate(v70, (char*)"..\\..\\src\\player.c", 11459);
				charAnimInfo->Motion->mdata = 0;
			}
			v71 = charAnimInfo->Motion;
			if (v71)
			{
				v72 = (AllocatedMem*)&v71[-1].type;
				MemoryManager__Deallocate(v72, (char*)"..\\..\\src\\player.c", 11464);
				charAnimInfo->Motion = 0;
			}
		}
		return;
	}
}

static void __declspec(naked) PlayAnimationThingASM()
{
	__asm
	{
		push esi
		call PlayAnimationThing_r
		pop esi
		retn
	}
}


void PatchAnimations()
{
	//WriteJump((void*)0x4692A0, PlayAnimationThingASM);
	WriteData((NJS_MOTION***)0x750A94, &TailsAnimations[0].Animation);
	WriteData((NJS_MOTION***)0x750182, &TailsAnimations[0].Animation);
	WriteCall((void*)0x74D02B, LoadTailsMTN);
	WriteCall((void*)0x74D352, PSetTailsMotion);
	WriteCall((void*)0x74D03C, PSetTailsMotion);
}