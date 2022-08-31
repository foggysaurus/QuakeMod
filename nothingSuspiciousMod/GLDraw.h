#pragma once

#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <gl\GL.h>
#include <unordered_map>

#include "ent.h"
#include "GLtext.h"
#include "vec.h"

#pragma comment(lib, "OpenGL32.lib")

namespace rgb
{
	const GLubyte red[3] = { 225, 10, 10 };
	const GLubyte pink[3] = { 140, 78, 80};
	const GLubyte green[3] = { 1, 225, 20};
	const GLubyte yellow[3] = { 255, 204, 0 };
	const GLubyte lightYellow[3] = { 255, 204, 102 };
	const GLubyte lightGreen[3] = { 0, 80, 40};
	const GLubyte gray[3] = { 55, 55, 55 };
	const GLubyte lightGray[3] = { 192, 192, 192 };
	const GLubyte black[3] = { 0, 0, 0 };
}

namespace GL
{

	// SetupOrtho - openGL functions
	void SetupOrtho();

	// RestoreGL - openGL functions
	void RestoreGL();

	// DrawFilledRect - draw filled rectangle
	void DrawFilledRect(float x, float y, float width, float height, const GLubyte color[3]);

	// DrawOutline - draw outline
	void DrawOutline(float x, float y, float width, float height, float lineWidth, const GLubyte color[3]);

	// DrawLine - draw line
	void DrawLine(float fromX, float fromY, float toX, float toY, float lineWidth, const GLubyte color[3]);

	// AddText - draw text
	void AddText(HDC& currentHDC, float textX, float textY, float textWidth, float textheight, const GLubyte* color, ColorInfo colorInfo, const char* text, bool& rebuildFont);

	// AddLargeText - draw large text
	void AddLargeText(HDC& currentHDC, float textX, float textY, float textWidth, 
		float textheight, const GLubyte* color, ColorInfo colorInfo, const char* text, bool& rebuildFont);

	// DrawEverything - draw everything at once
	void DrawEverything(std::vector<std::shared_ptr<Ent>>& entsSelected, std::shared_ptr<Me> me, 
		ScreenData screen, std::unordered_map <int, int>& varsMap, ColorInfo &colorInfo, 
		UIInfo &uiInfo, bool predictDraw, bool &rebuildFont);

	// DrawEverything - draw everything at once
	void DrawLinesToPlayers(vec2_t x, vec2_t y, float thickness, const GLubyte color[3]);

	// DrawBox - draw a bounding rectangle
	void DrawBox(vec2_t feet, vec2_t head, float thickness, const GLubyte color[3]);
}
