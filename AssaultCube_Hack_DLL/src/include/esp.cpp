#include "esp.h"


ESP::ESP(uintptr_t moduleBase)
{
	this->moduleBase = moduleBase;

	this->matrix = (float*)(moduleBase + 0x00192078);
	//17DFFC - 192078 - 17DFF8
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

	//if (ent)
	//{
	//	if (ent->vtable == (void*)0x4e4a98 || ent->vtable == (void*)0x4E4AC0)
	//	{
	//		return true;
	//	}
	//	else return false;
	//}
	//else return false;
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
		if (IsValidEntity(this->entityListPointer->ents[i]) && this->entityListPointer->ents[i] != nullptr)
		{
			Entity* ent = this->entityListPointer->ents[i];

			vec3 center = ent->headPos;

			// height adjustment to center of player model
			center.z = center.z - (EYE_HEIGHT + PLAYER_HEIGHT / 2);

			vec3 screenCoords;

			if (WorldToScreen(center, screenCoords, this->matrix, viewport[2], viewport[3]))
			{
				DrawESPBox(ent, screenCoords, font);
			}

		}
	}
}
