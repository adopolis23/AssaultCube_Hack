#include "glText.h"

/// <summary>
/// Builds the font to use for drawing, stores the old font for restoration later
/// </summary>
/// <param name="height"></param>
void GL::Font::Build(int height)
{
	hdc = wglGetCurrentDC();
	base = glGenLists(96);
	HFONT hFont = CreateFontA(-height, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "Consolas");

	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

	wglUseFontBitmaps(hdc, 32, 96, base);
	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);

	bBuilt = true;

}

void GL::Font::Print(float x, float y, const unsigned char color[3], const char* format, ...)
{
	glColor3ub(color[0], color[1], color[2]);
	glRasterPos2f(x, y);

	char text[100];
	va_list args;

	va_start(args, format);
	vsprintf_s(text, 100, format, args);
	va_end(args);

	glPushAttrib(GL_LIST_BIT);
	glListBase(base - 32);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

vec3 GL::Font::CenterText(float x, float y, float width, float height, float textWidth, float textHeight)
{
	vec3 textPos;
	textPos.x = x + (width - textWidth) / 2;
	textPos.y = y + textHeight;
	return textPos;
}

float GL::Font::CenterText(float x, float width, float textWidth)
{
	if (width > textWidth)
	{
		float diff = width - textWidth;
		return x + (diff / 2);
	}
	else
	{
		float diff = textWidth - width;
		return x - (diff / 2);
	}
}