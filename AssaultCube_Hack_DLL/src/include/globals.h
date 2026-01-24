#pragma once

#include "gameDefines.h"

// global runtime initiailized variables
struct Globals
{
	// base address of the game module
	uintptr_t moduleBase;

	// pointer to the combination of the view matrix and projection matrix
	float* ViewProjectionMatrix;

	// pointer to the entity list (only single player maybe)
	EntityList* entityListPointer;

	// pointer to the local player entity
	Entity* localPlayer;

	// number of bots in the current game (not counting player)
	uintptr_t* numBotsAddress;

	// number of total players in multiplayer game (including player)
	uintptr_t* numPlayerMpAddress;

	const int VIRTUAL_SCREEN_WIDTH = 1024;
	const int GAME_UNIT_MAGIC = 400;

	const float PLAYER_HEIGHT = 5.25f;
	const float PLAYER_WIDTH = 2.0f;
	const float EYE_HEIGHT = 4.5f;

	const float PLAYER_ASPECT_RATIO = PLAYER_HEIGHT / PLAYER_WIDTH;

	const int ESP_FONT_HEIGHT = 15;
	const int ESP_FONT_WIDTH = 9;

	const int PI = 3.14159265;

	inline void Initialize(uintptr_t modBase)
	{

		moduleBase = modBase;

		ViewProjectionMatrix = (float*)(moduleBase + 0x0017DFD0);

		entityListPointer = *(EntityList**)(moduleBase + 0x0018AC04);

		localPlayer = *(Entity**)(moduleBase + 0x0017E0A8);

		numBotsAddress = (uintptr_t*)(moduleBase + 0x00191FD4);

		numPlayerMpAddress = (uintptr_t*)(moduleBase + 0x001837E0);
	}

	inline void RefreshEntityList()
	{
		entityListPointer = *(EntityList**)(moduleBase + 0x0018AC04);
	}
};

extern Globals globals;



inline bool IsTeamGame()
{
	// need to find game mode pointer and implement this
	return true;
}

inline bool IsEnemy(Entity* ent)
{
	if (globals.localPlayer->Team == ent->Team)
		return false;
	else
		return true;
}

inline bool IsValidEntity(Entity* ent)
{
	// need to figure out if this actually works or find better way
	if (!ent)
		return false;

	MEMORY_BASIC_INFORMATION mbi{};
	if (!VirtualQuery(ent, &mbi, sizeof(mbi)))
		return false;

	if (mbi.State != MEM_COMMIT)
		return false;

	if (mbi.Protect & PAGE_GUARD)
		return false;

	if (mbi.Protect & PAGE_NOACCESS)
		return false;

	return true;

}