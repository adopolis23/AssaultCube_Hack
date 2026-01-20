#include "esp.h"


ESP::ESP()
{

}



bool ESP::IsTeamGame()
{
	// need to find game mode pointer and implement this
	return true;
}

bool ESP::IsEnemy(Entity* ent)
{
	if (g_Game.localPlayer->Team == ent->Team)
		return false;
	else
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

}


void ESP::DrawESPBox(Entity* ent, vec3 screen, GL::Font& font)
{
	const GLubyte* color = nullptr;

	if (IsTeamGame() && IsEnemy(ent)) color = RGB::red;
	else color = RGB::green;

	float dist = g_Game.localPlayer->bodyPos.Distance(ent->bodyPos);

	float scale = (g_Game.GAME_UNIT_MAGIC / dist) * (viewport[2] / g_Game.VIRTUAL_SCREEN_WIDTH);

	float x = screen.x - scale;
	float y = screen.y - (scale * g_Game.PLAYER_ASPECT_RATIO);

	float width = scale * 2.0f;
	float height = scale * g_Game.PLAYER_ASPECT_RATIO * 2.0f;

	GL::DrawOutlinedRect(x, y, width, height, 2.0f, color);

	float textX = font.CenterText(x, width, g_Game.ESP_FONT_WIDTH * strlen(ent->Name));
	float textY = y - g_Game.ESP_FONT_HEIGHT - 2.0f;

	font.Print(textX, textY, color, "%s", ent->Name);
}


void ESP::DrawESPHealthBar(Entity* ent, vec3 screen)
{
	int health = ent->Health;
	int death = 100 - health;

	if (health > 100) return;
	if (death > 100) return;

	const GLubyte* healthColor = RGB::green;
	const GLubyte* deathColor = RGB::red;

	float dist = g_Game.localPlayer->bodyPos.Distance(ent->bodyPos);
	if (dist > 400) return;

	float scale = (g_Game.GAME_UNIT_MAGIC / dist) * (viewport[2] / g_Game.VIRTUAL_SCREEN_WIDTH);

	float width = scale * 2.0f;
	float maxHeight = scale * g_Game.PLAYER_ASPECT_RATIO * 2.0f;

	float healthScaledHeight = scale * g_Game.PLAYER_ASPECT_RATIO * 2.0f * ((float)health / 100.0);
	float deathScaledHeight = scale * g_Game.PLAYER_ASPECT_RATIO * 2.0f * ((float)death / 100.0);

	GL::DrawFilledRect(screen.x + (width/2) + 100 / dist, screen.y - healthScaledHeight + (maxHeight / 2), 100/dist, healthScaledHeight, healthColor);
	GL::DrawFilledRect(screen.x + (width/2) + 100 / dist, screen.y - maxHeight + (maxHeight / 2), 100/dist, deathScaledHeight, deathColor);
}


void ESP::Draw(GL::Font& font)
{
	g_Game.RefreshEntityList();

	if (g_Game.entityListPointer == nullptr)
	{
		return;
	}

	glGetIntegerv(GL_VIEWPORT, viewport);

	for (int i = 1; i < 32; i++)
	{
		if (IsValidEntity(g_Game.entityListPointer->ents[i]) && g_Game.entityListPointer->ents[i] != nullptr)
		{
			Entity* ent = g_Game.entityListPointer->ents[i];

			vec3 center = ent->headPos;

			// height adjustment to center of player model
			center.z = center.z - (g_Game.EYE_HEIGHT / 3);

			vec3 screenCoords;

			if (WorldToScreen(center, screenCoords, g_Game.ViewProjectionMatrix, viewport[2], viewport[3]))
			{
				DrawESPBox(ent, screenCoords, font);
				DrawESPHealthBar(ent, screenCoords);
			}

		}
	}
}

void ESP::TestDraw(GL::Font& font)
{

	glGetIntegerv(GL_VIEWPORT, viewport);

	for (int i = 1; i < 32; i++)
	{
		if (IsValidEntity(g_Game.entityListPointer->ents[i]))
		{

			Entity* ent = g_Game.entityListPointer->ents[i];
			vec3 headPos = ent->bodyPos;

			vec3 screenCoords;

			if (WorldToScreen(headPos, screenCoords, g_Game.ViewProjectionMatrix, viewport[2], viewport[3]))
			{
				GL::DrawOutlinedRect(screenCoords.x, screenCoords.y, 30, 30, 2.0f, RGB::green);
			}

			

		}
	}
}
