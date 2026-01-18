#pragma once
#include <cstdint>
#include "geometry.h"

// class defines from reclass based on structure of memory at runtime

class clip
{
public:
	int32_t ammo;
};

class weapon
{
public:
	char pad_0000[16]; //0x0000
	int* reserveAmmo; //0x0010
	class clip* clip; //0x0014
	char pad_0018[1064]; //0x0018
}; //Size: 0x0440

class PlayerEnt
{
public:
	Vector3 headPos; //0x0004
	char pad_0010[24]; //0x0010
	Vector3 bodyPos; //0x0028
	Vector2 camAngle; //0x0034
	char pad_003C[58]; //0x003C
	int8_t NoClip; //0x0076
	int8_t Invis; //0x0077
	char pad_0078[116]; //0x0078
	int32_t Health; //0x00EC
	int32_t Armor; //0x00F0
	char pad_00F4[273]; //0x00F4
	char Name[16]; //0x0205
	char pad_0215[347]; //0x0215 -- 347
	class weapon* currWeaponPtr; //0x0370

	virtual void Function0();
	virtual void Function1();
	virtual void Function2();
	virtual void Function3();
	virtual void Function4();
	virtual void Function5();
	virtual void Function6();
	virtual void Function7();
	virtual void Function8();
	virtual void Function9();
}; //Size: 0x0D68

struct entList
{
	PlayerEnt* ents[31];
};
