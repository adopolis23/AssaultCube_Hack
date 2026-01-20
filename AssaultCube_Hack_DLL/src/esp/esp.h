#pragma once
#include <windows.h>
#include "glDraw.h"
#include "glText.h"
#include "gameDefines.h"
#include "globals.h"


class ESP
{
public:

	ESP();

	float* matrix; // address of view matrix in memory

	Entity* localPlayer; // address of local player in memory
	EntityList* entityListPointer; // address of entity list in memory

	int viewport[4];

	bool IsTeamGame();
	bool IsEnemy(Entity* ent);
	bool IsValidEntity(Entity* ent);

	void DrawESPBox(Entity* ent, vec3 screen, GL::Font& font);
	void DrawESPHealthBar(Entity* ent, vec3 screen);

	void Draw(GL::Font& font);
	void TestDraw(GL::Font& font);

private:
	uintptr_t moduleBase;
};