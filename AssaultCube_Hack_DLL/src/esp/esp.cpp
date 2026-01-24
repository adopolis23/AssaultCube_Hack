#include "esp.h"


ESP::ESP()
{

}





void ESP::DrawESPBox(Entity* ent, vec3 screen, GL::Font& font)
{
	const GLubyte* color = nullptr;

	if (IsTeamGame() && IsEnemy(ent)) color = RGB::red;
	else color = RGB::green;

	float dist = globals.localPlayer->bodyPos.Distance(ent->bodyPos);

	float scale = (globals.GAME_UNIT_MAGIC / dist) * (viewport[2] / globals.VIRTUAL_SCREEN_WIDTH);

	float x = screen.x - scale;
	float y = screen.y - (scale * globals.PLAYER_ASPECT_RATIO);

	float width = scale * 2.0f;
	float height = scale * globals.PLAYER_ASPECT_RATIO * 2.0f;

	GL::DrawOutlinedRect(x, y, width, height, 2.0f, color);

	float textX = font.CenterText(x, width, globals.ESP_FONT_WIDTH * strlen(ent->Name));
	float textY = y - globals.ESP_FONT_HEIGHT - 2.0f;

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

	float dist = globals.localPlayer->bodyPos.Distance(ent->bodyPos);
	if (dist > 400) return;

	float scale = (globals.GAME_UNIT_MAGIC / dist) * (viewport[2] / globals.VIRTUAL_SCREEN_WIDTH);

	float width = scale * 2.0f;
	float maxHeight = scale * globals.PLAYER_ASPECT_RATIO * 2.0f;

	float healthScaledHeight = scale * globals.PLAYER_ASPECT_RATIO * 2.0f * ((float)health / 100.0);
	float deathScaledHeight = scale * globals.PLAYER_ASPECT_RATIO * 2.0f * ((float)death / 100.0);

	GL::DrawFilledRect(screen.x + (width/2) + 100 / dist, screen.y - healthScaledHeight + (maxHeight / 2), 100/dist, healthScaledHeight, healthColor);
	GL::DrawFilledRect(screen.x + (width/2) + 100 / dist, screen.y - maxHeight + (maxHeight / 2), 100/dist, deathScaledHeight, deathColor);
}


void ESP::Draw(GL::Font& font)
{
	globals.RefreshEntityList();

	if (globals.entityListPointer == nullptr)
	{
		return;
	}

	glGetIntegerv(GL_VIEWPORT, viewport);

	for (int i = 1; i < 32; i++)
	{
		if (IsValidEntity(globals.entityListPointer->ents[i]) && globals.entityListPointer->ents[i] != nullptr)
		{
			Entity* ent = globals.entityListPointer->ents[i];

			vec3 center = ent->headPos;

			// height adjustment to center of player model
			center.z = center.z - (globals.EYE_HEIGHT / 3);

			vec3 screenCoords;

			if (WorldToScreen(center, screenCoords, globals.ViewProjectionMatrix, viewport[2], viewport[3]))
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
		if (IsValidEntity(globals.entityListPointer->ents[i]))
		{

			Entity* ent = globals.entityListPointer->ents[i];
			vec3 headPos = ent->bodyPos;

			vec3 screenCoords;

			if (WorldToScreen(headPos, screenCoords, globals.ViewProjectionMatrix, viewport[2], viewport[3]))
			{
				GL::DrawOutlinedRect(screenCoords.x, screenCoords.y, 30, 30, 2.0f, RGB::green);
			}

			

		}
	}
}
