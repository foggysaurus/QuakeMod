#include "pch.h"
#include "GLtext.h"

void GLTEXT::Font::Build(int height, bool bold, bool italic, std::string font)
{
	hdc = wglGetCurrentDC();
	base = glGenLists(96);

	int thickness = 400;
	LPCSTR fontNew = "Ariel";
	fontNew = (LPCSTR)font.c_str();
	if (bold)
		thickness = 700;

	HFONT hfont = CreateFontA(-height, 0, 0, 0, thickness, italic, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_DONTCARE | DEFAULT_PITCH, fontNew);
	//HFONT hfont = CreateFont(48, 0, 0, 0, FW_MEDIUM, FALSE, TRUE, FALSE, ANSI_CHARSET, OUT_OUTLINE_PRECIS,
	//	CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));

	HFONT hOldFont = (HFONT)SelectObject(hdc, hfont);
	wglUseFontBitmaps(hdc, 32, 96, base);
	SelectObject(hdc, hOldFont);
	DeleteObject(hfont);
	bBuilt = true;

} 

//-------------------------------------------------------------//
void  GLTEXT::Font::Print(float x, float y, const unsigned char color[3], const char* format, ...)
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
	//std::cout << (text) << std::endl;
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

//-------------------------------------------------------------//
vec3  GLTEXT::Font::CenterText(float x, float y, float width, float height, float textWidth, float textHeight)
{
	vec3 text;
	text.x = x - (width - textWidth) / 4;
	text.y = y - textHeight/2;
	return text;
}

//-------------------------------------------------------------//
float  GLTEXT::Font::CenterText(float x, float width, float textWidth)
{
	if (width > textWidth)
	{
		float difference = width - textWidth;
		return (x + (difference / 2));
	}
	else
	{
		float difference = textWidth - width;
		return (x - (difference / 2));
	}
}