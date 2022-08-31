#include "pch.h"
#include "GLDraw.h"
	//
GLTEXT::Font glFont;

int fontHeight = 16;
int fontWidth = 9;


GLTEXT::Font glFontLarge;
int fontHeightLarge = fontHeight*2;
int fontWidthLarge = fontWidth*2;

void GL::SetupOrtho()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	GLint viewPort[4];

	glDisable(GL_BLEND);
	glGetIntegerv(GL_VIEWPORT, viewPort);
	glViewport(0, 0, viewPort[2], viewPort[3]);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, viewPort[2], viewPort[3], 0, -1, 1);
}

//-------------------------------------------------------------//
void GL::RestoreGL()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();
}

//-------------------------------------------------------------//
void GL::DrawFilledRect(float x, float y, float width, float height, const GLubyte color[3])
{
	glColor3ub(color[0], color[1], color[2]);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
}

//-------------------------------------------------------------//
void GL::DrawOutline(float x, float y, float width, float height, float lineWidth, const GLubyte color[3])
{
	glLineWidth(lineWidth);
	glBegin(GL_LINE_STRIP);
	glColor3ub(color[0], color[1], color[2]);
	glVertex2f(x - 0.5f, y - 0.5f);
	glVertex2f(x + 0.5f + width, y - 0.5f);
	glVertex2f(x + 0.5f + width, y + height + 0.5f);
	glVertex2f(x - 0.5f, y + height + 0.5f);
	glVertex2f(x - 0.5f, y - 0.5f);
	glEnd();

}

//-------------------------------------------------------------//
void GL::DrawLine(float fromX, float fromY, float toX, float toY, float lineWidth, const GLubyte color[3]) {
	glLineWidth(lineWidth);
	glBegin(GL_LINES);
	glColor3ub(color[0], color[1], color[2]);
	glVertex2f(fromX, fromY);
	glVertex2f(toX, toY);
	glEnd();
}

//-------------------------------------------------------------//
void GL::AddText(HDC &currentHDC, float textX, float textY, float textWidth, float textheight, const GLubyte* color, ColorInfo colorInfo, 
	const char* text, bool &rebuildFont)
{
	if (!glFont.bBuilt || currentHDC != glFont.hdc || rebuildFont)
	{
		rebuildFont = false;
		glFont.Build(fontHeight, colorInfo.bold, colorInfo.italic, colorInfo.font);
	}

	glFont.Print(textX, textY, color, "%s", text);

}

//-------------------------------------------------------------//
void GL::AddLargeText(HDC& currentHDC, float textX, float textY, float textWidth, float textheight, 
	const GLubyte* color, ColorInfo colorInfo, const char* text, bool& rebuildFont)
{
	if (!glFontLarge.bBuilt || currentHDC != glFontLarge.hdc || rebuildFont)
	{
		glFontLarge.Build(fontHeightLarge, colorInfo.bold, colorInfo.italic, colorInfo.font);
	}

	glFontLarge.Print(textX, textY, color, "%s", text);
}

//-------------------------------------------------------------//
void GL::DrawEverything(std::vector<std::shared_ptr<Ent>>& entsSelected, std::shared_ptr<Me> me, 
	ScreenData screen, std::unordered_map <int, int>& varsMap, ColorInfo &colorInfo, UIInfo &uiInfo, bool predictDraw, bool &rebuildFont)
{

	HDC currentHDC = wglGetCurrentDC();
	GL::SetupOrtho();
	bool rebuildLargeFont = rebuildFont;
	//lines and boxes

	for (std::shared_ptr<Ent>& player : entsSelected)
	{
		GLubyte color[3];
		float lineThickness = 1.0f;
		float boxThickness = 2.0f;

		float start[2];
		float end[2];

		auto sadasd = colorInfo.font;

		start[0] = player->info2d.screenFeet[0] - player->info2d.modelWidth / 2.0f;
		start[1] = player->info2d.screenFeet[1];
		end[0] = player->info2d.screenHead[0] - player->info2d.modelWidth / 2.0f;
		end[1] = player->info2d.screenHead[1];

		//colors
		if (player->isEnemy)
		{
			lineThickness = colorInfo.linesEnemyThickness;
			boxThickness = colorInfo.boxEnemyThickness;
			if (player->isVisible)
			{
				std::copy(std::begin(colorInfo.colorEnemy), std::end(colorInfo.colorEnemy), std::begin(color));
			}

			else
			{
				std::copy(std::begin(colorInfo.colorEnemyInvis), std::end(colorInfo.colorEnemyInvis), std::begin(color));
			}
			
		}
		else
		{
			lineThickness = colorInfo.linesAllyThickness;
			boxThickness = colorInfo.boxAllyThickness;
			if (player->isVisible)
			{
				std::copy(std::begin(colorInfo.colorAlly), std::end(colorInfo.colorAlly), std::begin(color));
			}

			else
			{
				std::copy(std::begin(colorInfo.colorAllyInvis), std::end(colorInfo.colorAllyInvis), std::begin(color));
			}

		}
		if (player->isFriend)
		{
			lineThickness = colorInfo.linesFriendThickness;
			boxThickness = colorInfo.boxFriendThickness;
			if (player->isVisible)
			{
				std::copy(std::begin(colorInfo.colorFriend), std::end(colorInfo.colorFriend), std::begin(color));
			}

			else
			{
				std::copy(std::begin(colorInfo.colorFriendInvis), std::end(colorInfo.colorFriendInvis), std::begin(color));
			}

		}
		if (player->isTarget)
		{
			lineThickness = colorInfo.linesTargetThickness;
			boxThickness = colorInfo.boxTargetThickness;
			if (player->isVisible)
			{
				std::copy(std::begin(colorInfo.colorTarget), std::end(colorInfo.colorTarget), std::begin(color));
			}
			else
			{
				std::copy(std::begin(colorInfo.colorTargetInvis), std::end(colorInfo.colorTargetInvis), std::begin(color));
			}

		}


		//lines
		if (player->info2d.screenCrd[0] != 0 
			&& player->info2d.screenCrd[1] != 0  
			//&& player->anglesDistances.angleTo > 30 
			&& varsMap[VAR_DRAWLINES])
		{
			if (
				(varsMap[VAR_DRAWLINES_TOALLIES] && player->isAlly && !player->isFriend && !player->isTarget) 
				|| (varsMap[VAR_DRAWLINES_TOENEMIES] && player->isEnemy && !player->isTarget )
				|| (varsMap[VAR_DRAWLINES_FRIENDS] && player->isFriend && !player->isTarget )
				|| (varsMap[VAR_DRAWLINES_TARGETS] && player->isTarget)
				)
			{
				glLineWidth(lineThickness);
				glColor3ub(color[0], color[1], color[2]);
				glBegin(GL_LINES);
				glVertex2f(player->info2d.screenMyPos[0], player->info2d.screenMyPos[1]);
				glVertex2f(player->info2d.screenCrd[0], player->info2d.screenCrd[1]);
				glEnd();
			}


		}
		//boxes
		if (abs(start[0]) < screen.res[0] && abs(start[1]) < screen.res[1] && varsMap[VAR_DRAWBOXES])//&& 3 == 2)
		{
			if (player->entClass == CL_DEKA)
				player->info2d.modelWidth *= 2;
			glLineWidth(boxThickness);
			glColor3ub(color[0], color[1], color[2]);
			//glBegin(GL_LINE_STRIP);
			//glVertex2f(start[0] - 0.5f, start[1] - 0.5f);
			//glVertex2f(start[0] + 0.5f + modelWidth, start[1] - 0.5f);
			// 
			//glVertex2f(end[0] + 0.5f + modelWidth, end[1] + 0.5f);
			//glVertex2f(end[0] - 0.5f, end[1] + 0.5f);
			//glVertex2f(start[0] - 0.5f, start[1] - 0.5f);
			if ((varsMap[VAR_DRAWBOXES_ALLIES] && player->isAlly && !player->isFriend && !player->isTarget)
				|| varsMap[VAR_DRAWBOXES_ENEMIES] && player->isEnemy  && !player->isTarget
				|| varsMap[VAR_DRAWBOXES_FRIENDS] && player->isFriend && !player->isTarget
				|| varsMap[VAR_DRAWBOXES_TARGETS] && player->isTarget
				)
			{
				glBegin(GL_LINE_STRIP);
				glVertex2f(start[0] - 0.5f, start[1] - 0.5f);
				glVertex2f(start[0] + 0.5f + player->info2d.modelWidth / 4, start[1] - 0.5f);
				glEnd();

				glBegin(GL_LINE_STRIP);
				glVertex2f(start[0] + 0.5f + player->info2d.modelWidth * 0.75, start[1] - 0.5f);
				glVertex2f(start[0] + 0.5f + player->info2d.modelWidth, start[1] - 0.5f);
				glEnd();


				glBegin(GL_LINE_STRIP);
				glVertex2f(start[0] + 0.5f + player->info2d.modelWidth * 0.75, start[1] - 0.5f);
				glVertex2f(start[0] + 0.5f + player->info2d.modelWidth, start[1] - 0.5f);
				glVertex2f(end[0] + 0.5f + player->info2d.modelWidth, end[1] + 0.5f);
				glVertex2f(end[0] - 0.5f + player->info2d.modelWidth * 0.75, end[1] + 0.5f);
				glEnd();

				glBegin(GL_LINE_STRIP);
				glVertex2f(end[0] - 0.5f + player->info2d.modelWidth * 0.25, end[1] + 0.5f);
				glVertex2f(end[0] - 0.5f, end[1] + 0.5f);
				glVertex2f(start[0] - 0.5f, start[1] - 0.5f);
				glEnd();
			}


			//glEnd();
		}
	}

	//text

	for (std::shared_ptr<Ent>& player : entsSelected)
	{
		bool drawAllyStats = varsMap[VAR_DRAW_ALLY_STATS];
		bool drawEnemyStats = varsMap[VAR_DRAW_ENEMY_STATS];
		bool drawBigStats = varsMap[VAR_DRAW_BIG_STATS];
		

		GLubyte color[3];
		float end[2];

		end[0] = player->info2d.screenHead[0] - player->info2d.modelWidth / 2.0f;
		end[1] = player->info2d.screenHead[1];

		//colortext copy
		if (player->isEnemy)
		{
			if (player->isVisible)
			{
				std::copy(std::begin(colorInfo.colorEnemy), std::end(colorInfo.colorEnemy), std::begin(color));
			}
			else
			{
				std::copy(std::begin(colorInfo.colorEnemyInvis), std::end(colorInfo.colorEnemyInvis), std::begin(color));
			}
		}
		else
		{
			if (player->isVisible)
			{
				std::copy(std::begin(colorInfo.colorAlly), std::end(colorInfo.colorAlly), std::begin(color));
			}
			else
			{
				std::copy(std::begin(colorInfo.colorAllyInvis), std::end(colorInfo.colorAllyInvis), std::begin(color));
			}
		}
		if (player->isFriend)
		{
			if (player->isVisible)
			{
				std::copy(std::begin(colorInfo.colorFriend), std::end(colorInfo.colorFriend), std::begin(color));
			}
			else
			{
				std::copy(std::begin(colorInfo.colorFriendInvis), std::end(colorInfo.colorFriendInvis), std::begin(color));
			}
		}
		if (player->isTarget)
		{
			if (player->isVisible)
			{
				std::copy(std::begin(colorInfo.colorTarget), std::end(colorInfo.colorTarget), std::begin(color));
			}
			else
			{
				std::copy(std::begin(colorInfo.colorTargetInvis), std::end(colorInfo.colorTargetInvis), std::begin(color));
			}
		}


		std::string n = player->shortName;
		std::string id = std::to_string(player->id);
		id += ": " + n;
		char const* idname = id.c_str();


		std::string h = std::to_string(player->hp);
		char const* hp;
		hp = h.c_str();
	
		fontHeight = colorInfo.fontSize;
		fontHeightLarge = colorInfo.fontSize * 2;
		float newLine = colorInfo.fontSize;// +fontHeight / 4;

		if (player->isClosest )
		{
			if (((player->isAlly && drawAllyStats) || (player->isEnemy && drawEnemyStats)) && player->anglesDistances.angleTo < 8)
			{
				AddText(currentHDC, end[0] + player->info2d.modelWidth, end[1] - fontHeight, 0, 0, color, colorInfo, player->className, rebuildFont);
				AddText(currentHDC, end[0] + 5, end[1], 0, 0, color, colorInfo, idname, rebuildFont);
				AddText(currentHDC, end[0] + 5, end[1] + 1 * newLine, 0, 0, color, colorInfo, hp, rebuildFont);
			}

			if (drawBigStats)
			{
				AddLargeText(currentHDC, screen.res[0] - screen.res[0] / 6, screen.res[1] - screen.res[1] / 4, 0, 0, color, colorInfo, hp, rebuildLargeFont);
				AddLargeText(currentHDC, screen.res[0] - screen.res[0] / 6, screen.res[1] - screen.res[1] / 4 - fontHeightLarge, 0, 0, color, colorInfo, idname, rebuildLargeFont);
				AddLargeText(currentHDC, screen.res[0] - screen.res[0] / 6, screen.res[1] - screen.res[1] / 4 - 2 * fontHeightLarge, 0, 0, color, colorInfo, player->className, rebuildLargeFont);
			}

			break;
		}

	}

	//menu
	const GLubyte* tkColor;
	const GLubyte* wallsColor;
	const GLubyte* hitscanColor;

	std::string tk = "TK: ";
	if (uiInfo.isTK)
	{
		tk += "ON";
		tkColor = rgb::green;
	}
	else
	{
		tk += "OFF";
		tkColor = rgb::red;
	}


	auto writeTK = tk.c_str();

	std::string walls = "Check walls: ";
	if (uiInfo.isThroughWalls)
	{
		walls += "OFF";
		wallsColor = rgb::red;
	}
	else
	{
		walls += "ON";
		wallsColor = rgb::green;
	}

	auto writeWalls = walls.c_str();

	std::string hitscan = "Hitscan: ";
	if (uiInfo.isHitscan)
	{
		hitscan += "OFF";
		hitscanColor = rgb::red;
	}

	else
	{
		hitscan += "ON";
		hitscanColor = rgb::green;
	}

	auto writeHit = hitscan.c_str();

	float floatX = uiInfo.posX;
	float Xcrd = screen.res[0] * (floatX / 100);

	float floatY = uiInfo.posY;
	float Ycrd = screen.res[1] * (floatY / 100);

	AddText(currentHDC, Xcrd, Ycrd, 0, 0, tkColor, colorInfo, writeTK, rebuildFont);
	AddText(currentHDC, Xcrd, Ycrd + fontHeight, 0, 0, wallsColor, colorInfo, writeWalls, rebuildFont);
	AddText(currentHDC, Xcrd, Ycrd + 2*fontHeight, 0, 0, hitscanColor, colorInfo, writeHit, rebuildFont);

	//colors
	//
	if (uiInfo.correctColor1)
		GL::DrawFilledRect(Xcrd, Ycrd - 2*fontHeight, 9, 16, rgb::yellow);
	else
		GL::DrawFilledRect(Xcrd, Ycrd - 2 * fontHeight, 9, 16, rgb::lightGreen);
	if (uiInfo.correctColor2)
		GL::DrawFilledRect(Xcrd + 12, Ycrd - 2 * fontHeight, 9, 16, rgb::yellow);
	else
		GL::DrawFilledRect(Xcrd + 12, Ycrd - 2 * fontHeight, 9, 16, rgb::lightGreen);
	if (uiInfo.correctColor3)
		GL::DrawFilledRect(Xcrd + 24, Ycrd - 2 * fontHeight, 9, 16, rgb::yellow);
	else
		GL::DrawFilledRect(Xcrd + 24, Ycrd - 2 * fontHeight, 9, 16, rgb::lightGreen);
	if (uiInfo.correctColor4)
		GL::DrawFilledRect(Xcrd + 36, Ycrd - 2 * fontHeight, 9, 16, rgb::yellow);
	else
		GL::DrawFilledRect(Xcrd + 36, Ycrd - 2 * fontHeight, 9, 16, rgb::lightGreen);



	GL::RestoreGL();
}

//-------------------------------------------------------------//
void GL::DrawLinesToPlayers(vec2_t x, vec2_t y, float thickness, const GLubyte color[3])
{
	if (y[0] && y[1])
	{
		HDC currentHDC = wglGetCurrentDC();
		GL::SetupOrtho();

		GL::DrawLine(x[0], x[1], y[0], y[1], thickness, color);

		GL::RestoreGL();
	}
}

//-------------------------------------------------------------//
void GL::DrawBox(vec2_t feet, vec2_t head, float thickness, const GLubyte color[3])
{
	float modelHeight = head[1] - feet[1];
	float modelWidth = modelHeight / 2.0f;
	float start[2];
	start[0] = feet[0] - modelWidth / 2.0f;
	start[1] = feet[1];

	HDC currentHDC = wglGetCurrentDC();
	GL::SetupOrtho();
	GL::DrawOutline(start[0], start[1], modelWidth, modelHeight, thickness, color);
	GL::RestoreGL();
}