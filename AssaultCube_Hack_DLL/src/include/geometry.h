#pragma once

#include <cmath>

struct Vector2
{
	float x;
	float y;

	inline float Distance(const Vector2& other) const
	{
		float dx = x - other.x;
		float dy = y - other.y;
		return std::sqrt(dx * dx + dy * dy);
	}
};


struct Vector3
{
	float x;
	float y;
	float z;

	inline float Distance(const Vector3& other) const
	{
		float dx = x - other.x;
		float dy = y - other.y;
		float dz = z - other.z;
		return std::sqrt(dx * dx + dy * dy + dz * dz);
	}
};

struct Vector4 : Vector3
{
	float x;
	float y;
	float z;
	float w;
};


// using definitions
using vec4 = Vector4;
using vec3 = Vector3;
using vec2 = Vector2;
using vec = Vector4;


inline bool WorldToScreen(const vec3 pos, vec3& screen, float matrix[16], int windowWidth, int windowHeight)
{
	vec4 clipCoords;

	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
	clipCoords.y = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
	clipCoords.z = pos.x * matrix[2] + pos.y * matrix[6] + pos.z * matrix[10] + matrix[14];
	clipCoords.w = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];

	if (clipCoords.w < 0.1f)
	{
		return false;
	}
	vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (NDC.x + 1.0f) * 0.5f * windowWidth;
	screen.y = (1.0f - NDC.y) * 0.5f * windowHeight;
	screen.z = NDC.z;

	return true;
}

inline void MultiplyMatrix(const float* a, const float* b, float* out)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			out[i + j * 4] =
				a[0 + j * 4] * b[i + 0 * 4] +
				a[1 + j * 4] * b[i + 1 * 4] +
				a[2 + j * 4] * b[i + 2 * 4] +
				a[3 + j * 4] * b[i + 3 * 4];
		}
	}
}