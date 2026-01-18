#include "esp.h"


ESP::ESP(uintptr_t moduleBase)
{
	this->moduleBase = moduleBase;

	this->entityListPointer = *(EntityList**)(moduleBase + 0x0018AC04);
	this->localPlayer = *(Entity**)(moduleBase + 0x0017E0A8);
}



bool ESP::IsTeamGame()
{
	// need to find game mode pointer and implement this
	return true;
}

bool ESP::IsEnemy(Entity* ent)
{
	// need to find team pointer and check if local player team matches entity team
	return true;
}

bool ESP::IsValidEntity(Entity* ent)
{
	// if ent->vtable == 0x4e4a98 || 0x4e4ac0 
	// checks if the vtable is valid
	return true;
}


void ESP::DrawESPBox(Entity* ent, vec3 screen, GL::Font& font)
{
	const GLubyte* color = nullptr;

	if (IsTeamGame() && IsEnemy(ent)) color = RGB::red;
	else color = RGB::green;

	float dist = localPlayer->bodyPos.Distance(ent->bodyPos);

	float scale = (GAME_UNIT_MAGIC / dist) * (viewport[2] / VIRTUAL_SCREEN_WIDTH);

	float x = screen.x - scale;
	float y = screen.y - (scale * PLAYER_ASPECT_RATIO); 

	float width = scale * 2.0f;
	float height = scale * PLAYER_ASPECT_RATIO * 2.0f;

	GL::DrawOutlinedRect(x, y, width, height, 2.0f, color);

	float textX = font.CenterText(x, width, ESP_FONT_WIDTH * strlen(ent->Name));
	float textY = y - ESP_FONT_HEIGHT - 2.0f;

	font.Print(textX, textY, color, "%s", ent->Name);
}

void ESP::Draw(GL::Font& font)
{
	glGetIntegerv(GL_VIEWPORT, viewport);

	for (int i = 1; i < 32; i++)
	{
		if (IsValidEntity(entityListPointer->ents[i]) && entityListPointer->ents[i] != nullptr)
		{
			Entity* ent = entityListPointer->ents[i];

			DrawESPBox(ent, vec3(), font);
		}
	}
}
