#pragma once
#include <windows.h>
#include "glDraw.h"
#include "glText.h"
#include "gameDefines.h"

const int VIRTUAL_SCREEN_WIDTH = 800;
const int GAME_UNIT_MAGIC = 400;

const float PLAYER_HEIGHT = 5.25f;
const float PLAYER_WIDTH = 2.0f;
const float EYE_HEIGHT = 4.5f;

const float PLAYER_ASPECT_RATIO = PLAYER_HEIGHT / PLAYER_WIDTH;

const int ESP_FONT_HEIGHT = 15;
const int ESP_FONT_WIDTH = 9;


class ESP
{
public:

	ESP(uintptr_t moduleBase);

	float* matrix; // address of view matrix in memory

	Entity* localPlayer; // address of local player in memory
	EntityList* entityListPointer; // address of entity list in memory

	int viewport[4];

	bool IsTeamGame();
	bool IsEnemy(Entity* ent);
	bool IsValidEntity(Entity* ent);

	void DrawESPBox(Entity* ent, vec3 screen, GL::Font& font);
	void Draw(GL::Font& font);

private:
	uintptr_t moduleBase;
};