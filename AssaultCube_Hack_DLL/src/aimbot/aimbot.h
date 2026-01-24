#pragma once
#include "globals.h"
#include <stdio.h>

namespace aimbot 
{
	vec3 CalcAngle(vec3 src, vec3 dst);

	Entity* GetBestTarget();

	// main aimbot method
	// finds best target gets new angle and applies to player
	void Aimbot();
}

