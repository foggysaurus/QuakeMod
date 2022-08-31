#pragma once
#pragma comment(lib, "OpenGL32.lib")
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <gl\GL.h>
#include <wingdi.h>
#include "vec.h"

struct vec3 
{ 
	float x;
	float y;
	float z;
};

namespace GLTEXT
{
	class Font
	{
	public:
		bool bBuilt = false;
		unsigned int base;
		HDC hdc = nullptr;
		int height;
		int width;

	public:

		// Build - build font
		void Build(int height, bool bold, bool italic, std::string font);

		// Print - write text
		void Print(float x, float y, const unsigned char color[3], const char* format, ...);

		// CenterText - center text
		vec3 CenterText(float x, float y, float width, float height, float textWidth, float textHeight);
		float CenterText(float x, float width, float textWidth);

	};
}
