#pragma once
#include "globals.h"

/// <summary>
/// Calculate the angle difference between two 3D vectors
/// </summary>
/// <param name="src"></param>
/// <param name="dst"></param>
/// <returns></returns>
vec3 CalcAngleDifference(vec3 src, vec3 dst)
{
	vec3 delta;
	delta.x = -atan2f(dst.x - src.x, dst.y - src.y) / globals.PI * 180.0f + 180.0f;
	delta.y = asinf((dst.z - src.z) / src.Distance(dst)) * 180.0f / globals.PI;
	delta.z = 0.0f;

	return delta;
}