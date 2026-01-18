#pragma once
#pragma comment(lib, "OpenGL32.lib")

#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include "mem.h"
#include "geometry.h"


namespace GL
{
	class Font
	{
	public:
		void Build(int height);
		void Print(float x, float y, const unsigned char color[3], const char* format, ...);

		vec3 CenterText(float x, float y, float width, float height, float textWidth, float textHeight);
		float CenterText(float x, float width, float textWidth);

		bool bBuilt = false;
		unsigned int base;
		HDC hdc = nullptr;
		int height;
		int width;
	};
}