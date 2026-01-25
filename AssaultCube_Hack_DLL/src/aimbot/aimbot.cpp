#include "aimbot.h"
#include <cfloat>

/// <summary>
/// Calculate the angle difference between two 3D vectors
/// </summary>
/// <param name="src"></param>
/// <param name="dst"></param>
/// <returns></returns>
vec3 aimbot::CalcAngle(vec3 src, vec3 dst)
{
	vec3 delta;
	delta.x = -atan2f(dst.x - src.x, dst.y - src.y) / globals.PI * 180.0f + 180.0f;
	delta.y = asinf((dst.z - src.z) / src.Distance(dst)) * 180.0f / globals.PI;
	delta.z = 0.0f;

	return delta;
}

Entity* aimbot::GetBestTarget()
{
	vec3 angle;
	float bestDist = FLT_MAX;
	float dist = 0;
	Entity* target = nullptr;

	globals.RefreshEntityList();

	if (globals.entityListPointer == nullptr)
	{
		return nullptr;
	}

	for (int i = 1; i <= (*(globals.numBotsAddress) + *(globals.numPlayerMpAddress)); i++)
	{
		if (IsValidEntity(globals.entityListPointer->ents[i]) && globals.entityListPointer->ents[i] != nullptr && IsEnemy(globals.entityListPointer->ents[i]))
		{
			if (globals.entityListPointer->ents[i]->Health <= 0)
				continue;

			angle = CalcAngle(globals.localPlayer->bodyPos, globals.entityListPointer->ents[i]->bodyPos);
			
			dist = globals.localPlayer->camAngle.Distance(angle);

			if (dist < bestDist)
			{
				bestDist = dist;
				target = globals.entityListPointer->ents[i];
			}

		}
	}

	return target;
}

void aimbot::Aimbot()
{
	// activate on right mouse button
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		const Entity* ent = GetBestTarget();
		if (!ent)
			return;

		printf("Entity name slected by aimbot is: %s\n", ent->Name);

		const vec3 angle = CalcAngle(globals.localPlayer->headPos, ent->headPos);

		globals.localPlayer->camAngle = angle;
	}
}