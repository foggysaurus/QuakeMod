#include "pch.h"
#include "actions.h"
#include "hook.h"

#include "memory.h"

#include "Wallhack.h"
#include "Globals.h"

HWND afkHD = NULL;

void SingletonGlobalVars::CreateMapping(std::string& buffer)
{
	HANDLE hFileMap;
	BOOL bResult;
	PCHAR lpBuffer = NULL;
	const char* Buffer = buffer.c_str();
	size_t szBuffer = (buffer.size() + 1);
	hFileMap = CreateFileMapping
	(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		256,
		(LPCSTR)L"Local\\MyFileMap"
	);
	if (hFileMap == FALSE)
	{
		std::cout << "epic fail" << std::endl;
	}

	lpBuffer = (PCHAR)MapViewOfFile
	(
		hFileMap,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		256
	);
	if (lpBuffer == NULL)
	{
		std::cout << "epic fail2" << std::endl;
	}
	CopyMemory(lpBuffer, Buffer, szBuffer);
	bResult = UnmapViewOfFile(lpBuffer);
	if (bResult == FALSE)
	{
		std::cout << "epic fail 3" << std::endl;
	}

}

//-------------------------------------------------------------//
bool SingletonGlobalVars::ReadFromMap(std::string& result)
{
	HANDLE hFileMap;
	BOOL bResult;
	PCHAR lpBuffer = NULL;
	hFileMap = OpenFileMapping
	(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		(LPCSTR)L"Local\\MyFileMap"
	);
	if (hFileMap == NULL)
	{
		std::cout << "epic fail4" << std::endl;
	}
	lpBuffer = (PCHAR)MapViewOfFile
	(
		hFileMap,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		256
	);
	if (lpBuffer == NULL)
	{
		std::cout << "epic fail5" << std::endl;
		return 0;

	}
	result = lpBuffer;
	bResult = UnmapViewOfFile(lpBuffer);
	if (bResult == NULL)
	{
		std::cout << "epic fail6" << std::endl;
	}
	CloseHandle(hFileMap);

}

//-------------------------------------------------------------//
bool SingletonGlobalVars::AfkDetector(int procId, HWND& afkHWND, int myId, int& leaderId, bool& isAfk)
{
	HWND curForegroundWindow = GetForegroundWindow();
	DWORD lpProc;
	GetWindowThreadProcessId(curForegroundWindow, &lpProc);
	if (procId != lpProc)
	{
		//execute once per afk window switch
		if (!oldAfkWindow)
		{
			oldAfkWindow = true;
		}
		oldActiveWindow = false;
		isAfk = true;
		return true;
	}
	else
	{
		if (!oldActiveWindow)
		{
			idCheck = myId;
			idStringCheck = std::to_string(myId);
			if (myId > -1 && myId < 33)
				CreateMapping(idStringCheck);
			else
				std::cout << "something wroong with id" << std::endl;
			oldActiveWindow = true;
		}
		oldAfkWindow = false;
		isAfk = false;
		return false;
	}
}

//-------------------------------------------------------------//
bool SingletonGlobalVars::IsKeyPressedOnce(int key, int modKey)
{
	bool bkey;
	bool bmod;
	if (modKey != 0)
	{
		bkey = (GetAsyncKeyState(key) & 1);
		bmod = GetAsyncKeyState(modKey);
		//bool what = bkey && bmod;
		return  bkey && bmod;
		//if (what)
		   //std::cout << what << std::endl;

		//return what;
	}
	else
	{
		return (GetAsyncKeyState(key) & 1);
	}
}

//-------------------------------------------------------------//
bool SingletonGlobalVars::IsKeyPressed(int key, int modKey)
{

	if (modKey != 0)
	{
		return (GetAsyncKeyState(key) && GetAsyncKeyState(modKey));
	}
	else
	{
		return (GetAsyncKeyState(key));
	}

}

//-------------------------------------------------------------//
void SingletonGlobalVars::FrameRate(std::chrono::time_point<std::chrono::steady_clock>& startFrame,
	std::chrono::time_point<std::chrono::steady_clock>& prevFrame, std::chrono::duration<float>& frameDuration,
	std::vector<float>& frames, int& slowFps)
{
	int fps = 1;
	startFrame = std::chrono::high_resolution_clock::now();
	frameDuration = startFrame - prevFrame;

	auto elapsedFrameTime = frameDuration.count() * 1000.0f;
	fps = 1000 / elapsedFrameTime;
	if (!fps)
		fps = 1;

	frames.push_back(fps);
	if (frames.size() > fps)
	{
		float sum = 0;
		for (float& frame : frames)
		{
			sum += frame;
		}
		slowFps = sum / frames.size();
		frames.clear();
	}
	//std::cout << fps << std::endl;
	prevFrame = startFrame;
}

//-------------------------------------------------------------//
HMODULE SingletonGlobalVars::GetModId(int procId, const char* modName)
{
	HMODULE hModule = NULL;
	wchar_t* testss = NULL;
	BYTE* test = 0;
	MODULEENTRY32 mod;
	mod.dwSize = sizeof(mod);
	HANDLE modSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, procId);
	if (Module32First(modSnap, &mod))
	{
		do
		{
			char* module = mod.szModule;
			std::string where = mod.szModule;
			std::string what = modName;
			if (where.find(what) != std::string::npos)
			{
				hModule = mod.hModule;
			}
		} while (Module32Next(modSnap, &mod));
	}
	return hModule;
}

//-------------------------------------------------------------//
void SingletonGlobalVars::SetAddresses(ModuleAddresses* moduleAddresses)
{
	uintptr_t curProc = GetCurrentProcessId();
	moduleAddresses->mbiiModuleBase = curProc;
	moduleAddresses->mbiiHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, curProc);
	moduleAddresses->ojkBase = (uintptr_t)GetModId(curProc, "OJK");
	moduleAddresses->openGLBase = (uintptr_t)GetModId(curProc, "OPENGL32");
	moduleAddresses->kernelBase = (uintptr_t)GetModId(curProc, "KERNELBASE");
	moduleAddresses->rdVanilla = (uintptr_t)GetModId(curProc, "vanilla");
	moduleAddresses->mbiiBase = (uintptr_t)GetModId(curProc, "mbii.x86.exe");
}

//-------------------------------------------------------------//
bool SingletonGlobalVars::WExists(const std::wstring& name)
{
	FILE* peFile = NULL;
	const wchar_t* result = name.c_str();
	_wfopen_s(&peFile, result, L"rb");
	if (peFile == NULL)
	{
		//fclose(peFile);
		return false;
	}
	fclose(peFile);
	return true;
}

//-------------------------------------------------------------//
std::string SingletonGlobalVars::GetCurrentDirectory()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}

//-------------------------------------------------------------//
std::wstring SingletonGlobalVars::GetDesktopPath()
{
	wchar_t* p;
	if (S_OK != SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &p)) return L"";
	std::wstring result(p);
	CoTaskMemFree(p);
	return result;
}

//-------------------------------------------------------------//
std::string SingletonGlobalVars::ScanConsole(uintptr_t& addressCons, uintptr_t& addressOffs)
{

	int consoleSize = *(int*)addressOffs;
	int msgSize = 0;
	if (consoleSize != oldConsoleSize)
	{
		msgSize = consoleSize - oldConsoleSize;
		if (msgSize > 4096 || msgSize < 0)
			msgSize = 0;

		if (msgSize != 0)
		{
			consoleMsg.clear();
			consoleMsg.reserve(msgSize);

			for (int i = 0; i < msgSize; i++)
			{
				char newChar = *(char*)(addressCons + oldConsoleSize + i);
				consoleMsg.push_back(newChar);
			}
		}
		oldConsoleSize = consoleSize;
	}
	return consoleMsg;
}

//-------------------------------------------------------------//
void SingletonGlobalVars::ReadBinds(std::string consoleLastLine)
{
	if (size_t loc = consoleLastLine.find("bind ") != std::string::npos)
	{
		std::string tempSubstring = consoleLastLine.substr(loc + 5);
		size_t nameLoc = tempSubstring.find_first_of(" ");
		std::string name = tempSubstring.substr(0, nameLoc);
		size_t valueLoc = tempSubstring.find_first_of("\n");
		std::string value = tempSubstring.substr(nameLoc + 1, valueLoc - nameLoc - 1);
		auto scan = VkKeyScanExA(value.c_str()[0], GetKeyboardLayout(0));
		auto vkCode = scan & 0xff;
		auto shift = (scan & 0x100) > 0;
		auto ctrl = (scan & 0x200) > 0;
		auto alt = (scan & 0x400) > 0;
		//std::cout << name << ": " << value << ": " << vkCode << ": " << alt << std::endl;

	}
}

//-------------------------------------------------------------//
int SingletonGlobalVars::FromStringToIntSafe(std::string in, int base)
{
	int returnValue = -1;
	try
	{
		returnValue = std::stol(in, nullptr, base);
		return returnValue;
	}
	catch (...)
	{
		std::cout << "Config file formatting error: " + in << std::endl;
		return -1;
	}
}

//-------------------------------------------------------------//
float SingletonGlobalVars::FromStringToFloatSafe(std::string in)
{
	float returnValue = -1;
	try
	{
		returnValue = std::stof(in);
		return returnValue;
	}
	catch (...)
	{
		std::cout << "Config file formatting error" << std::endl;
		return -1;
	}
}

//-------------------------------------------------------------//
void SingletonGlobalVars::FillSavedNames(std::vector<std::string>& savedPlayerNames, std::wstring hackConfigPath)
{
	std::wstring winput;
	std::wifstream inFile(hackConfigPath.c_str());
	while (std::getline(inFile, winput))
	{
		std::string input(winput.begin(), winput.end());
		savedPlayerNames.push_back(input);
	}
}

//-------------------------------------------------------------//
bool SingletonGlobalVars::CheckToAppendFast(std::vector<std::string>& savedPlayerNames, std::wstring hackConfigPath,
	std::string lineToCheck)
{
	std::ofstream ifile;
	std::wstring winput;
	std::wifstream inFile(hackConfigPath.c_str());
	bool isAppend = true;

	for (std::string& playerName : savedPlayerNames)
	{
		if ((playerName == lineToCheck))
		{
			isAppend = false;
			return 0;
		}
	}

	ifile.open(hackConfigPath, std::ios_base::app | std::ios_base::in);
	{
		savedPlayerNames.push_back(lineToCheck);

		ifile << lineToCheck + "\n";
		ifile.close();
	}
	return 1;
}

//-------------------------------------------------------------//
bool SingletonGlobalVars::CheckToAppend(std::wstring hackConfigPath, std::string lineToCheck, int defaultLevel)
{
	std::wstring winput;
	std::string origLine = lineToCheck;
	std::wifstream inFile(hackConfigPath.c_str());

	if (lineToCheck != "\n")
		while (lineToCheck.find('\n') != std::string::npos)
		{

			lineToCheck = lineToCheck.erase(lineToCheck.find('\n'), 1);
		}

	while (std::getline(inFile, winput))
	{
		std::string input(winput.begin(), winput.end());
		if (input.find(lineToCheck) != std::string::npos)
		{
			return false;
		}
	}
	lineToCheck = origLine;


	std::ofstream ifile;

	char errBuffer[100];
	ifile.open(hackConfigPath, std::ios_base::app | std::ios_base::in);
	{
		if (defaultLevel != -1)
			ifile << lineToCheck + " " + std::to_string(defaultLevel) + "\n";
		else
			ifile << lineToCheck + "\n";
		ifile.close();
	}
	return 0;
}

//-------------------------------------------------------------//
bool SingletonGlobalVars::CheckToAppend(std::wstring hackConfigPath, std::string lineToCheck, double defaultLevel)
{
	std::wstring winput;
	std::wifstream inFile(hackConfigPath.c_str());
	while (std::getline(inFile, winput))
	{
		std::string input(winput.begin(), winput.end());
		if (input.find(lineToCheck) != std::string::npos)
		{
			return false;
		}
	}
	std::ofstream ifile;

	char errBuffer[100];
	ifile.open(hackConfigPath, std::ios_base::app | std::ios_base::in);
	{
		if (defaultLevel != -1)
			ifile << lineToCheck + " " + std::to_string(defaultLevel) + "\n";
		else
			ifile << lineToCheck + "\n";
		ifile.close();
	}
	return 0;
}

//-------------------------------------------------------------//
bool SingletonGlobalVars::CheckToAppend(std::wstring hackConfigPath, std::string lineToCheck, std::string extra)
{
	std::wstring winput;
	std::wifstream inFile(hackConfigPath.c_str());
	while (std::getline(inFile, winput))
	{
		std::string input(winput.begin(), winput.end());
		if (input.find(lineToCheck) != std::string::npos)
		{
			return false;
		}
	}
	std::ofstream ifile;

	char errBuffer[100];
	ifile.open(hackConfigPath, std::ios_base::app | std::ios_base::in);
	{
		ifile << lineToCheck + " " + extra + "\n";
		ifile.close();
	}
	return 0;
}

//-------------------------------------------------------------//
void SingletonGlobalVars::WriteCfg(std::wstring hackConfigPath, std::vector<std::string>& commands)
{

	CheckToAppend(hackConfigPath, "default: aim F9, TK toggle ctrl + t, close hack ctrl + F1", -1);
	CheckToAppend(hackConfigPath, "bind aim_key", "0x78");
	CheckToAppend(hackConfigPath, "bind aim_mod", "0");
	CheckToAppend(hackConfigPath, "bind aim_noPred_key", "0xA2");
	CheckToAppend(hackConfigPath, "bind aim_noPred_mod", "0");
	CheckToAppend(hackConfigPath, "bind teamKill_key", "0x54");
	CheckToAppend(hackConfigPath, "bind teamKill_mod", "0xA2");
	CheckToAppend(hackConfigPath, "bind closeCheat_key", "0x70");
	CheckToAppend(hackConfigPath, "bind closeCheat_mod", "0xA2");
	CheckToAppend(hackConfigPath, "bind stopTargetingMe_key", "0x4C");
	CheckToAppend(hackConfigPath, "bind stopTargetingMe_mod", "0xA2");

	CheckToAppend(hackConfigPath, "bind targetThroughWalls_key", "0x57");
	CheckToAppend(hackConfigPath, "bind targetThroughWalls_mod", "0x12");

	CheckToAppend(hackConfigPath, "set lockToAim", 0);
	CheckToAppend(hackConfigPath, "set wallhackLevel", 0);
	CheckToAppend(hackConfigPath, "set switchTargetOnKill", 1);
	CheckToAppend(hackConfigPath, "set autoTargetjedi", 1);
	CheckToAppend(hackConfigPath, "set targetOnlyHighlight", 0);
	CheckToAppend(hackConfigPath, "set aimExclusivelyAtTargetUntilItsDead", 0);
	CheckToAppend(hackConfigPath, "set smoothAim", 0);
	CheckToAppend(hackConfigPath, "set autoPB", 1);
	CheckToAppend(hackConfigPath, "set antiAfk", 1);
	CheckToAppend(hackConfigPath, "set toggleToHitscan", 0);
	CheckToAppend(hackConfigPath, "set autoPunish", 1);

	CheckToAppend(hackConfigPath, "set antiKnockback", 1);
	CheckToAppend(hackConfigPath, "\n//-----------------------------------------OPENGL CONFIG----------------------------------------//\n", -1);
	CheckToAppend(hackConfigPath, "\n//-------ENABLE LINES BOXES TEXT-------//\n", -1);
	CheckToAppend(hackConfigPath, "set boxes", 1);
	CheckToAppend(hackConfigPath, "set drawBoxesAllies", 1);
	CheckToAppend(hackConfigPath, "set drawBoxesEnemies", 1);
	CheckToAppend(hackConfigPath, "set drawBoxesFriends", 1);
	CheckToAppend(hackConfigPath, "set drawBoxesTargets", 1);
	CheckToAppend(hackConfigPath, "set lines", 1);
	CheckToAppend(hackConfigPath, "set linesAllies", 1);
	CheckToAppend(hackConfigPath, "set linesEnemies", 1);
	CheckToAppend(hackConfigPath, "set linesFriends", 1);
	CheckToAppend(hackConfigPath, "set linesTargets", 1);
	CheckToAppend(hackConfigPath, "set displayEnemyStats", 1);
	CheckToAppend(hackConfigPath, "set displayAllyStats", 1);
	CheckToAppend(hackConfigPath, "set displayBigStats", 1);
	CheckToAppend(hackConfigPath, "\n//-------LINES AND BOXES-------//\n", -1);
	CheckToAppend(hackConfigPath, "set linesEnemyThickness", 1.0);
	CheckToAppend(hackConfigPath, "set linesAllyThickness", 1.0);
	CheckToAppend(hackConfigPath, "set linesFriendThickness", 1.0);
	CheckToAppend(hackConfigPath, "set boxEnemyThickness", 1.0);
	CheckToAppend(hackConfigPath, "set linesTargetThickness", 1.0);
	CheckToAppend(hackConfigPath, "set boxAllyThickness", 1.0);
	CheckToAppend(hackConfigPath, "set boxFriendThickness", 1.0);
	CheckToAppend(hackConfigPath, "set boxTargetThickness", 1.0);
	CheckToAppend(hackConfigPath, "\n//-------TEXT-------//\n", -1);
	CheckToAppend(hackConfigPath, "setStrs Font", "Arial");
	CheckToAppend(hackConfigPath, "set fontSize", 16);
	CheckToAppend(hackConfigPath, "set bold", 1);
	CheckToAppend(hackConfigPath, "set italic", 0);
	CheckToAppend(hackConfigPath, "set uiStatsX", 10.0);
	CheckToAppend(hackConfigPath, "set uiStatsY", 70.0);
	CheckToAppend(hackConfigPath, "\n//-------COLORS-------//\n", -1);
	CheckToAppend(hackConfigPath, "Colors enemyboxes", "(255, 51, 0)");
	CheckToAppend(hackConfigPath, "Colors allyboxes", "(0, 153, 0)");
	CheckToAppend(hackConfigPath, "Colors friendboxes", "(255, 204, 0)");
	CheckToAppend(hackConfigPath, "Colors enemyBoxesInvis", "(255, 102, 102)");
	CheckToAppend(hackConfigPath, "Colors allyBoxesInvis", "(153, 255, 153)");
	CheckToAppend(hackConfigPath, "Colors friendBoxesInvis", "(255, 204, 102)");
	CheckToAppend(hackConfigPath, "Colors targetBoxes", "(135, 30, 191)");
	CheckToAppend(hackConfigPath, "Colors targetBoxesInvis", "(153, 100, 188)");

	commands.push_back("aim_key");
	commands.push_back("aim_mod");
}

//-------------------------------------------------------------//
void SingletonGlobalVars::ReadCfg(std::wstring hackPath, std::unordered_map <int, KeyModKey>& keyMap, std::unordered_map <int, int>& varsMap,
	ColorInfo& colorInfo, UIInfo& uinfo, bool& rebuildFont)
{
	std::wfstream readBinds(hackPath);
	std::wstring winput;
	KeyModKey KeysMods{ -1,-1 };
	int varValue = 0;
	float varValueFloat = 0;
	const char* varStr;

	std::wifstream inFile(hackPath.c_str());

	while (std::getline(inFile, winput))
	{
		std::string input(winput.begin(), winput.end());
		if (input.find("bind") != std::string::npos)
		{
			auto val = input.substr(input.find_last_of(" "));

			if (input.find("aim_key") != std::string::npos)
			{
				KeysMods.key = FromStringToIntSafe(val, 16);
			}
			if (input.find("aim_mod") != std::string::npos)
			{
				KeysMods.modkey = FromStringToIntSafe(val, 16);
				keyMap[CFG_AIM] = KeysMods;
			}
			if (input.find("aim_noPred_key") != std::string::npos)
			{
				KeysMods.key = FromStringToIntSafe(val, 16);
			}
			if (input.find("aim_noPred_mod") != std::string::npos)
			{
				KeysMods.modkey = FromStringToIntSafe(val, 16);
				keyMap[CFG_AIM_NOPRED] = KeysMods;
			}
			if (input.find("teamKill_key") != std::string::npos)
			{
				KeysMods.key = FromStringToIntSafe(val, 16);
			}
			if (input.find("teamKill_mod") != std::string::npos)
			{
				KeysMods.modkey = FromStringToIntSafe(val, 16);
				keyMap[CFG_TK] = KeysMods;
			}
			if (input.find("closeCheat_key") != std::string::npos)
			{
				KeysMods.key = FromStringToIntSafe(val, 16);

			}
			if (input.find("closeCheat_mod") != std::string::npos)
			{
				KeysMods.modkey = FromStringToIntSafe(val, 16);
				keyMap[CFG_EXIT] = KeysMods;
			}

			if (input.find("targetThroughWalls_key") != std::string::npos)
			{
				KeysMods.key = FromStringToIntSafe(val, 16);

			}
			if (input.find("targetThroughWalls_mod") != std::string::npos)
			{
				KeysMods.modkey = FromStringToIntSafe(val, 16);
				keyMap[CFG_THROUGHWALLS] = KeysMods;
			}

		}
		if (input.find("set") != std::string::npos)
		{

			if (input.find(".") == std::string::npos)
			{

				auto firstSpace = input.find_first_of(" ");
				auto lastSpace = input.find_last_of(" ");
				auto val = input.substr(lastSpace + 1);
				auto variable = input.substr(firstSpace + 1, lastSpace - firstSpace - 1);
				if (variable == "lockToAim")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_LOCK] = varValue;
				}
				if (variable == "wallhackLevel")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_WALLHACK] = varValue;
				}
				if (variable == "switchTargetOnKill")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_SWITCHONKILL] = varValue;
				}
				if (variable == "autoTargetjedi")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_AUTOTARGETJEDI] = varValue;
				}
				if (variable == "antiKnockback")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_ANTIKNOCKBACK] = varValue;
				}
				if (variable == "boxes")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_DRAWBOXES] = varValue;
				}
				if (variable == "drawBoxesAllies")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_DRAWBOXES_ALLIES] = varValue;
				}
				if (variable == "drawBoxesEnemies")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_DRAWBOXES_ENEMIES] = varValue;
				}
				if (variable == "lines")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_DRAWLINES] = varValue;
				}
				if (variable == "linesAllies")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_DRAWLINES_TOALLIES] = varValue;
				}
				if (variable == "linesEnemies")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_DRAWLINES_TOENEMIES] = varValue;
				}
				if (variable == "linesTargets")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_DRAWLINES_TARGETS] = varValue;
				}
				if (variable == "displayEnemyStats")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_DRAW_ENEMY_STATS] = varValue;
				}
				if (variable == "displayAllyStats")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_DRAW_ALLY_STATS] = varValue;
				}
				if (variable == "displayBigStats")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_DRAW_BIG_STATS] = varValue;
				}
				if (variable == "linesFriends")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_DRAWLINES_FRIENDS] = varValue;
				}
				if (variable == "drawBoxesFriends")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_DRAWBOXES_FRIENDS] = varValue;
				}
				if (variable == "drawBoxesTargets")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_DRAWBOXES_TARGETS] = varValue;
				}
				if (variable == "smoothAim")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_SMOOTHAIM] = varValue;
				}
				if (variable == "targetOnlyHighlight")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_AIMTARGET] = varValue;
				}
				if (variable == "aimExclusivelyAtTargetUntilItsDead")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_TARGETEXCLUSIVE] = varValue;
				}
				if (variable == "autoPB")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_AUTOPB] = varValue;
				}
				if (variable == "antiAfk")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_ANTIAFK] = varValue;
				}
				if (variable == "toggleToHitscan")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_TOGGLETOHITSCAN] = varValue;
				}
				if (variable == "autoPunish")
				{
					varValue = FromStringToIntSafe(val, 10);
					varsMap[VAR_AUTOPUNISH] = varValue;
				}
				if (variable == "fontSize")
				{
					rebuildFont = true;
					varValue = FromStringToIntSafe(val, 10);
					colorInfo.fontSize = varValue;
				}
				if (variable == "bold")
				{
					rebuildFont = true;
					varValue = FromStringToIntSafe(val, 10);
					colorInfo.bold = varValue;
				}
				if (variable == "italic")
				{
					rebuildFont = true;
					varValue = FromStringToIntSafe(val, 10);
					colorInfo.italic = varValue;
				}



			}
			if (input.find(".") != std::string::npos)
			{
				auto firstSpace = input.find_first_of(" ");
				auto lastSpace = input.find_last_of(" ");
				auto val = input.substr(lastSpace + 1);
				auto variable = input.substr(firstSpace + 1, lastSpace - firstSpace - 1);

				if (variable == "linesEnemyThickness")
				{
					varValueFloat = FromStringToFloatSafe(val);
					colorInfo.linesEnemyThickness = varValueFloat;
				}
				if (variable == "linesAllyThickness")
				{
					varValueFloat = FromStringToFloatSafe(val);
					colorInfo.linesAllyThickness = varValueFloat;
				}
				if (variable == "linesFriendThickness")
				{
					varValueFloat = FromStringToFloatSafe(val);
					colorInfo.linesFriendThickness = varValueFloat;
				}
				if (variable == "linesTargetThickness")
				{
					varValueFloat = FromStringToFloatSafe(val);
					colorInfo.linesTargetThickness = varValueFloat;
				}
				if (variable == "boxEnemyThickness")
				{
					varValueFloat = FromStringToFloatSafe(val);
					colorInfo.boxEnemyThickness = varValueFloat;
				}
				if (variable == "boxAllyThickness")
				{
					varValueFloat = FromStringToFloatSafe(val);
					colorInfo.boxAllyThickness = varValueFloat;
				}
				if (variable == "boxFriendThickness")
				{
					varValueFloat = FromStringToFloatSafe(val);
					colorInfo.boxFriendThickness = varValueFloat;
				}
				if (variable == "boxTargetThickness")
				{
					varValueFloat = FromStringToFloatSafe(val);
					colorInfo.boxTargetThickness = varValueFloat;
				}
				if (variable == "uiStatsX")
				{
					varValueFloat = FromStringToFloatSafe(val);
					uinfo.posX = varValueFloat;
				}
				if (variable == "uiStatsY")
				{
					varValueFloat = FromStringToFloatSafe(val);
					uinfo.posY = varValueFloat;
				}

			}

		}
		if (input.find("setStr") != std::string::npos)
		{
			if (input.find("Font") != std::string::npos)
			{
				auto val = input.substr(input.find_last_of(" ") + 1);
				colorInfo.font = val;
			}
		}
		if (input.find("Colors") != std::string::npos)
		{
			GLubyte colors[3];

			size_t locVal1 = input.find("(") + 1;
			size_t locVal2 = input.find_first_of(",") + 1;
			size_t locVal3 = input.find_last_of(",") + 1;
			size_t end = input.find(")") + 1;


			auto firstSpace = input.find_first_of(" ");
			auto firstPar = input.find_first_of("(");
			auto variable = input.substr(firstSpace + 1, firstPar - firstSpace - 2);

			auto val1 = input.substr(locVal1, locVal2 - locVal1);
			colors[0] = FromStringToIntSafe(val1, 10);
			auto val2 = input.substr(locVal2, locVal3 - locVal2);
			colors[1] = FromStringToIntSafe(val2, 10);
			auto val3 = input.substr(locVal3, end - locVal3);
			colors[2] = FromStringToIntSafe(val3, 10);

			if (variable == "enemyboxes")
			{
				std::copy(std::begin(colors), std::end(colors), std::begin(colorInfo.colorEnemy));
			}
			if (variable == "enemyBoxesInvis")
			{
				std::copy(std::begin(colors), std::end(colors), std::begin(colorInfo.colorEnemyInvis));
			}
			if (variable == "allyboxes")
			{
				std::copy(std::begin(colors), std::end(colors), std::begin(colorInfo.colorAlly));
			}
			if (variable == "allyBoxesInvis")
			{
				std::copy(std::begin(colors), std::end(colors), std::begin(colorInfo.colorAllyInvis));
			}
			if (variable == "friendboxes")
			{
				std::copy(std::begin(colors), std::end(colors), std::begin(colorInfo.colorFriend));
			}
			if (variable == "friendBoxesInvis")
			{
				std::copy(std::begin(colors), std::end(colors), std::begin(colorInfo.colorFriendInvis));
			}
			if (variable == "targetBoxes")
			{
				std::copy(std::begin(colors), std::end(colors), std::begin(colorInfo.colorTarget));
			}
			if (variable == "targetBoxesInvis")
			{
				std::copy(std::begin(colors), std::end(colors), std::begin(colorInfo.colorTargetInvis));
			}

		}
	}
}

//-------------------------------------------------------------//
bool SingletonGlobalVars::GetCommand(std::wstring& playerNamesPath, std::string lastLineOfConsole, std::vector<int>& targetIds,
	std::vector<int>& friendIds, std::shared_ptr<Me> me, std::vector<std::string>& savedPlayerNames,
	std::vector<std::shared_ptr<Ent>>& ents, ModuleAddresses* modules, AddressesList& addressesList,
	bool isAfk)
{
	if (prevConsoleInput != lastLineOfConsole)
	{
		prevConsoleInput = lastLineOfConsole;
	}
	else
		return 0;



	std::string myCommand = "]";
	std::string myCommandChat = "\n" + me->name + "^7: ^2"; //]asd asd\n
	std::string myVoiceCommand = "\n<" + me->name + "^7:";

	int targetId = -1;
	int friendId = -1;

	auto firstSpace = lastLineOfConsole.find_first_of(" ");
	//playerlistfill


	if (lastLineOfConsole.find("^7 connected") != std::string::npos && !isAfk)
	{
		auto editLine = lastLineOfConsole;
		size_t pos = lastLineOfConsole.find("^7 connected");
		size_t lastButOneNewLine = 0;
		if (editLine.find_last_of('\n') != std::string::npos)
		{
			editLine = editLine.erase(editLine.find('\n'), 1);
			if (editLine.find_last_of('\n') != std::string::npos)
			{
				lastButOneNewLine = editLine.find_last_of('\n');
			}
		}


		auto tempName = lastLineOfConsole.substr(lastButOneNewLine, pos);

		if (pos != std::string::npos && tempName.length() > 1)
		{
			CheckToAppendFast(savedPlayerNames, playerNamesPath, tempName + "             " + GetShortName(tempName));
		}
	}

	if (lastLineOfConsole.find("\nServer settings:") != std::string::npos && !isAfk)
	{
		for (std::shared_ptr<Ent>& player : ents)
		{
			if (me->name != player->name && player->name.length() > 1)
			{
				CheckToAppendFast(savedPlayerNames, playerNamesPath, player->name + "             " + player->shortName);
			}
		}

	}


	if (!isAfk)
	{
		if (lastLineOfConsole.substr(0, firstSpace) == "]target")
			try
		{
			int idAttempt = -1;
			auto nameStartPos = lastLineOfConsole.find("target") + 7;
			std::string targetTempName;
			if (nameStartPos != std::string::npos)
			{
				targetTempName = lastLineOfConsole.substr(nameStartPos, lastLineOfConsole.length() - nameStartPos - 1);
			}

			idAttempt = std::stoi(targetTempName, nullptr);
			if (idAttempt >= 0 && idAttempt < 33 && idAttempt != me->id)
			{
				for (std::shared_ptr<Ent>& player : ents)
				{
					if (std::find(friendIds.begin(), friendIds.end(), idAttempt) == friendIds.end() && !player->isFriend)
					{
						if (std::find(targetIds.begin(), targetIds.end(), idAttempt) == targetIds.end() && !player->isFriend)
							targetIds.push_back(idAttempt);
					}

				}

			}
			else
			{
				targetId = -1;
			}
		}
		catch (...)
		{
			auto nameStartPos = lastLineOfConsole.find("target") + 7;
			std::string targetTempName;
			if (nameStartPos != std::string::npos)
			{
				targetTempName = lastLineOfConsole.substr(nameStartPos, lastLineOfConsole.length() - nameStartPos - 1);
			}


			for (std::shared_ptr<Ent>& player : ents)
			{
				if (targetId == -1 && player->shortName.find(targetTempName) != std::string::npos)
				{
					if (std::find(friendIds.begin(), friendIds.end(), player->id) == friendIds.end() && !player->isFriend)
					{
						if (std::find(targetIds.begin(), targetIds.end(), player->id) == targetIds.end() && !player->isFriend)
							targetIds.push_back(player->id);
					}

				}

			}
		}

		if (lastLineOfConsole.substr(0, firstSpace) == "]friend")
			try
		{
			int idAttempt = -1;
			auto nameStartPos = lastLineOfConsole.find("friend") + 7;
			std::string targetTempName;
			if (nameStartPos != std::string::npos)
			{
				targetTempName = lastLineOfConsole.substr(nameStartPos, lastLineOfConsole.length() - nameStartPos - 1);
			}

			idAttempt = std::stoi(targetTempName, nullptr);
			if (idAttempt >= 0 && idAttempt < 33 && idAttempt != me->id)
			{
				for (std::shared_ptr<Ent>& player : ents)
				{
					if (player->id == idAttempt)
					{
						if (std::find(targetIds.begin(), targetIds.end(), idAttempt) == targetIds.end() && !player->isFriend)
						{
							if (std::find(friendIds.begin(), friendIds.end(), idAttempt) == friendIds.end())
								friendIds.push_back(idAttempt);
						}

					}

				}

			}
			else
			{
				friendId = -1;
			}
		}
		catch (...)
		{
			auto nameStartPos = lastLineOfConsole.find("friend") + 7;
			std::string friendTempName;
			if (nameStartPos != std::string::npos)
			{
				friendTempName = lastLineOfConsole.substr(nameStartPos, lastLineOfConsole.length() - nameStartPos - 1);
			}


			for (std::shared_ptr<Ent>& player : ents)
			{
				if (targetId == -1 && player->shortName.find(friendTempName) != std::string::npos)
				{
					if (std::find(targetIds.begin(), targetIds.end(), player->id) == targetIds.end() && !player->isFriend)
					{
						if (std::find(friendIds.begin(), friendIds.end(), player->id) == friendIds.end() && !player->isFriend)
							friendIds.push_back(player->id);
					}
				}

			}
		}


		if (lastLineOfConsole == "]target -1\n")
		{
			targetIds.clear();
		}

		if (lastLineOfConsole == "]friend -1\n")
		{
			friendIds.clear();
		}
	}

}

//-------------------------------------------------------------//
std::string SingletonGlobalVars::GetMyName(uintptr_t address)
{
	const char* inputStr = (char*)(address);
	std::string output = inputStr;
	size_t fpLoc = output.find("name");
	if (fpLoc != std::string::npos)
	{
		output = output.substr(fpLoc + 5);
		output = output.substr(0, output.find_first_of("\\"));
	}
	return output;
}

//-------------------------------------------------------------//
std::string SingletonGlobalVars::GetName(std::shared_ptr<Ent> player, ModuleAddresses* modules,
	AddressesList& addressesList)
{
	const char* newName = (char*)(modules->ojkBase + 0x5205850 + player->id * addressesList.teamOffset); //5205850 //first 0x5205850
	newName = (char*)(modules->ojkBase + 0x5205850 + player->id * addressesList.teamOffset);
	std::string name(newName);

	return name;
}

//-------------------------------------------------------------//
std::string SingletonGlobalVars::GetShortName(std::string& name)
{
	std::string tempName = name;
	size_t pos = std::string::npos;
	for (int j = 0; j < 10; j++)
	{
		std::string pattern(("^" + std::to_string(j)));
		std::string::size_type n = 0;
		while ((n = tempName.find(pattern, n)) != std::string::npos)
		{
			tempName.replace(n, pattern.size(), "");
		}

	}
	return tempName;
}

//-------------------------------------------------------------//
void SingletonGlobalVars::ReturnHp(std::vector<std::shared_ptr<Ent>>& entsAlive)
{
	for (std::shared_ptr<Ent>& player : entsAlive)
	{
		if (!player->hp || (
			(player->entClass == CL_SOLD
				|| player->entClass == CL_SOLDIMP
				|| player->entClass == CL_ET
				|| player->entClass == CL_ETIMP
				|| player->entClass == CL_CLONE)
			&& player->hp > 200))
		{
			player->hp = 100;
		}
		if ((player->hp == 100) && (player->entClass == CL_SOLD || player->entClass == CL_SOLDIMP))
			player->hp = 50;
		if ((player->hp == 100) && (player->entClass == CL_CLONE || player->entClass == CL_ET
			|| player->entClass == CL_ETIMP))
			player->hp = 80;
		if ((player->hp == 100) && (player->entClass == CL_SBD))
			player->hp = 150;
		if ((player->hp == 100) && (player->entClass == CL_WOOK))
			player->hp = 300;
	}
}

//-------------------------------------------------------------//
void SingletonGlobalVars::FillArrays(std::vector<std::shared_ptr<Ent>>& ents, std::vector<std::shared_ptr<Ent>>& entsAlive,
	std::vector<std::shared_ptr<Ent>>& entsAllies, std::vector<std::shared_ptr<Ent>>& entsEnemies,
	std::shared_ptr<Me>& me,
	AddressesList& addressesList, ModuleAddresses* modules, int maxId)
{
	const char* classesNames[]{ "none", "sold",  "sold",  "et",  "et",  "sith",  "jedi",
"bh",  "hero",  "sbd",  "wook",  "deka",  "clone", "manda", "arc" };


	for (int i = 0; i <= maxId; i++)
	{
		std::shared_ptr<Ent> player(new Ent());

		player->FillEnt(modules->mbiiBase, addressesList.entBase + i * addressesList.playerOffset);
		player->team = *(int*)(addressesList.team + i * addressesList.teamOffset);
		player->entClass = *(int*)(addressesList.enClass + i * addressesList.enemyClassOffset);
		player->color1 = *(int*)(addressesList.color1 + i * addressesList.colorOffset);
		player->color2 = *(int*)(addressesList.color2 + i * addressesList.colorOffset);

		//if (player->team && player->weapon && player->isRendered)
		//	player->isDisrupted = true;
		//if (player->team && player->weapon && player->isRendered)
		//	player->entClass = 4;

		if (player->color1 == 65507)
			player->correctColor1 = true;
		if (player->color2 == 14352128)
			player->correctColor2 = true;
		if (player->color1 == 1573119)
			player->correctColor3 = true;
		if (player->color2 == 6911)
			player->correctColor4 = true;

		if (player->correctColor1 && player->correctColor2)
			player->isFriend = true;

		if ((player->color1 == 65507 && player->color2 == 14352128) || (player->color1 == 1573119 && player->color2 == 6911))
		{
			player->isFriend = true;
		}



		for (int i = 0; i < sizeof(classesNames) / sizeof(classesNames[0]); i++)
		{
			if (player->entClass == i)
			{
				player->className = classesNames[i];
			}
		}

		if (player->team && player->numWeapon && player->entClass && player->id != me->id && player->isRendered && player->isDisrupted)
		{
			entsAlive.push_back(player);
		}
		if (player->team)
			ents.push_back(player);
	}
	for (std::shared_ptr<Ent> player : entsAlive)
	{
		if (me->team == player->team || player->isFriend)
		{
			player->isAlly = true;
			entsAllies.push_back(player);
		}
		if (player->isFriend)
		{
			player->isFriend = true;
		}
		if (me->team != player->team)
		{
			player->isEnemy = true;
			entsEnemies.push_back(player);
		}
	}
	ReturnHp(entsAlive);
}

//-------------------------------------------------------------//
void SingletonGlobalVars::FillMe(std::vector<std::shared_ptr<Ent>>& ents, std::shared_ptr<Me>& me, AddressesList& addressesList,
	ModuleAddresses* modules, std::string& myName)
{
	vec3_t contactPoint;
	//heightDepth heightDepth;
	me->FillEnt(modules->mbiiBase, addressesList.entBase + *(int*)addressesList.trueId * addressesList.playerOffset);
	me->FillMe(addressesList, *(int*)addressesList.trueId, addressesList.playerOffset);
	me->id = me->trueId;
	me->team = *(int*)(addressesList.team + *(int*)addressesList.trueId * addressesList.teamOffset);
	me->entClass = *(int*)(addressesList.enClass + *(int*)addressesList.trueId * addressesList.enemyClassOffset);
	me->lastAttackerNum = *(int*)(addressesList.lastAttacker);
	for (std::shared_ptr<Ent> player : ents)
	{
		if (me->id == player->id)
		{
			me->name = player->name;
			me->shortName = player->shortName;
		}
	}

	VecCopy(me->groundContact, contactPoint);
	//myName = GetMyName(addressesList.myNameAdd);
	//me->ForcePowers(forcepowers);
	//me->name = myName;
}

//-------------------------------------------------------------//
void SingletonGlobalVars::FillNames(std::vector<std::shared_ptr<Ent>>& ents, ModuleAddresses* modules,
	std::vector<PlayerNames>& allPlayerNames, AddressesList& addressesList, int& newFrame, int& slowFps)
{
	if (newFrame % slowFps == 0)
	{
		allPlayerNames.clear();
		for (std::shared_ptr<Ent>& player : ents)
		{
			if (player->team)
			{
				PlayerNames playerName;
				playerName.id = player->id;
				playerName.name = GetName(player, modules, addressesList);
				playerName.shortName = GetShortName(playerName.name);
				allPlayerNames.push_back(playerName);
			}
		}

	}
}

//-------------------------------------------------------------//
void SingletonGlobalVars::FindDekas(AddressesList& addressesList, std::vector<int>& dekasList, int& newFrame, int& slowFps)
{

	if (newFrame % slowFps == 0)
	{
		dekasList.clear();
		for (int i = 0; i < 1025; i++)
		{
			int venHumTemp;
			venHumTemp = *(int*)((addressesList.vehiclesEntities + i * addressesList.playerOffset));
			if (venHumTemp == 13)
			{
				int alive = *(int*)(addressesList.vehiclesEntities + i * addressesList.playerOffset + 0x444);
				if (*(int*)(addressesList.vehiclesEntities + i * addressesList.playerOffset + 0x444))
					dekasList.push_back(i);


				std::ostringstream ss;
				ss << std::hex << (addressesList.vehiclesEntities + i * addressesList.playerOffset);
				std::string dekaAddress = ss.str();

				//std::cout << i << ": " << *(float*)(addressesList.vehiclesEntities + i * addressesList.playerOffset + 0x18) << ": " << dekaAddress << std::endl;
			}
		}
	}
}

//-------------------------------------------------------------//
void SingletonGlobalVars::AddDekas(std::vector<int>& dekasList, AddressesList& addressesList, std::vector<std::shared_ptr<Ent>>& entsAlive,
	std::shared_ptr<Me>& me)
{
	if (dekasList.size())
		for (int& dekaNumber : dekasList)
		{
			std::shared_ptr<Ent> deka(new Ent());
			//ent* deka = new ent();

			deka->FillDeka(addressesList.vehiclesEntities + dekaNumber * addressesList.playerOffset);
			deka->entClass = 11;
			deka->weapon = 4;
			deka->numWeapon = 4;
			deka->team = 2;


			if (deka->stance2 != 1426 && deka->id != me->id && deka->stance && deka->stance2 && deka->id < 33)
			{
				entsAlive.push_back(deka);
			}
		}
}

//-------------------------------------------------------------//
bool SingletonGlobalVars::AmIdead(std::shared_ptr<Me>& me)
{
	return  (me->team == 3 || !me->team || !me->weapon);
}

//-------------------------------------------------------------//
void SingletonGlobalVars::AutoPunish(std::shared_ptr<Ent>& ent, std::shared_ptr<Me>& me, ExecuteCommand& executeCommand)
{
	if (ent)
	{
		if (me->team == ent->team && me->tked && !ent->isFriend)
		{
			executeCommand.ExecOneTime("say !p");
			//std::cout << "!p" << ent->entClass << ": " << me->tked << ": " << me->lastAttackerNum << std::endl;
		}
		if (me->team == ent->team && me->tked && ent->isFriend)
		{
			executeCommand.ExecOneTime("say !f");
		}
	}
}

//-------------------------------------------------------------//
BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam)
{
	DWORD lpdwProcessId;
	HWND prevhandle = NULL;
	char name[12];
	std::string nameStr;
	std::vector<HWND> handles;
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);


	if (lpdwProcessId == lParam)
	{
		GetWindowTextA(hwnd, name, 12);
		nameStr = name;
		if (nameStr == "OpenJK (MP)")
		{
			afkHD = hwnd;
			std::cout << name << std::endl;
			return false;
		}
	}
	return true;
}

//-------------------------------------------------------------//
void SingletonGlobalVars::ExecOnStartOnce(FILE* f, ModuleAddresses* modules, std::wstring& hackConfigPath,
	std::string& execPath, std::wstring& playerNamesPath, ExecuteCommand& executeCommand,
	std::vector<std::string>& commands, std::unordered_map <int, KeyModKey>& keyMap,
	std::unordered_map <int, int>& varsMap, ColorInfo& colorInfo,
	UIInfo& uiInfo, bool& rebuildFont, std::vector<std::string>& savedPlayerNames, HWND& afkHandle)
{
	afkHandle = afkHD;
	AllocConsole();
	freopen_s(&f, "CONOUT$", "w", stdout);
	SetAddresses(modules);

	auto deskPath = GetDesktopPath();
	std::wstring HackStuffPath = deskPath + L"\\epicHax" + L"\\hackStuff.cfg";
	hackConfigPath = deskPath + L"\\epicHax" + L"\\hackConfig.cfg";
	execPath = GetCurrentDirectory() + "\\MBII\\1exec.cfg";
	playerNamesPath = deskPath + L"\\epicHax" + L"\\playerList.cfg";
	executeCommand.execPath = execPath;

	WriteCfg(hackConfigPath, commands);
	ReadCfg(hackConfigPath, keyMap, varsMap, colorInfo, uiInfo, rebuildFont);
	EnumWindows(EnumWindowsProcMy, modules->mbiiModuleBase);
	FillSavedNames(savedPlayerNames, playerNamesPath);

}

//-------------------------------------------------------------//
bool SingletonGlobalVars::LockFromExecuting(ModuleAddresses* modules, bool& isAddressCheckLocked)
{
	int locker = *(int*)(modules->mbiiBase + 0x646F90);
	int locker2 = *(int*)(modules->mbiiBase + 0x647110);

	if (locker != 0 && locker2 != 0)
	{
		if (!isAddressCheckLocked)
		{
			isAddressCheckLocked = true;
			modules->ojkBase = (uintptr_t)GetModId(modules->mbiiModuleBase, "OJK");
		}
		return false;
	}
	return true;
}

//-------------------------------------------------------------//
void SingletonGlobalVars::NewRound(AddressesList& addressesList, bool& newRound)
{
	int newMatch = *(int*)(addressesList.inMatch);
	if (!(newRound) && newMatch)
	{
		newRound = true;
	}
	if (newMatch == 0)
		newRound = false;
}

//-------------------------------------------------------------//
void SingletonGlobalVars::FindMaxId(const int maxPlayers, AddressesList& addressesList, int maxId)
{
	for (int i = 0; i < maxPlayers; i++)
	{
		int bufferId = *(int*)(addressesList.entBase + i * addressesList.playerOffset);
		if (bufferId > maxId)
			maxId = bufferId;
	}
}

//-------------------------------------------------------------//
void SingletonGlobalVars::FillPlayerData(const int& maxPlayers, AddressesList& addressesList, int& maxId, ModuleAddresses* modules,
	std::vector<std::shared_ptr<Ent>>& ents, std::vector<std::shared_ptr<Ent>>& entsAlive, std::vector<std::shared_ptr<Ent>>& entsAllies,
	std::vector<std::shared_ptr<Ent>>& entsEnemies, std::shared_ptr<Me>& me, std::vector<int>& dekasList,
	std::string& myName, int newFrame, int slowFps, std::vector<PlayerNames>& allPlayerNames, int myid,
	UIInfo& uiInfo)
{
	FindMaxId(maxPlayers, addressesList, maxId);

	FillMe(ents, me, addressesList, modules, myName);
	FindDekas(addressesList, dekasList, newFrame, slowFps);
	AddDekas(dekasList, addressesList, entsAlive, me);
	FillArrays(ents, entsAlive, entsAllies, entsEnemies, me, addressesList, modules, maxId);

	FillNames(ents, modules, allPlayerNames, addressesList, newFrame, slowFps);
	myid = me->id;

	//names attacker
	if (ents.size())
		for (std::shared_ptr<Ent>& player : ents)
		{
			//fill names
			if (allPlayerNames.size())
			{
				for (const PlayerNames& nameList : allPlayerNames)
				{
					if (player->id == nameList.id)
					{
						player->name = nameList.name;
						player->shortName = nameList.shortName;
						if (me->id == nameList.id)
							me->name = nameList.name;
					}
				}
			}
			//find last attacker
			if (player->id == me->lastAttackerNum && player->id != me->id && me->lastAttackerNum < 33)
				me->lastAttacker = player;
			if (player->id == me->id)
			{
				me->correctColor1 = player->correctColor1;
				me->correctColor2 = player->correctColor2;
				me->correctColor3 = player->correctColor3;
				me->correctColor4 = player->correctColor4;

				uiInfo.correctColor1 = me->correctColor1;
				uiInfo.correctColor2 = me->correctColor2;
				uiInfo.correctColor3 = me->correctColor3;
				uiInfo.correctColor4 = me->correctColor4;
			}
		}

}

//-------------------------------------------------------------//
void SingletonGlobalVars::FindingFriendsAndTargets(UIInfo& uiInfo, std::vector<int>& targetIds, std::vector<int>& friendIds,
	std::vector<std::shared_ptr<Ent>>& ents)
{
	uiInfo.targets.clear();
	if (friendIds.size() || targetIds.size())
		for (std::shared_ptr<Ent>& player : ents)
		{
			if (friendIds.size())
				for (int id : friendIds)
				{
					if (player->id == id)
						player->isFriend = true;
				}

			if (targetIds.size())
			{
				for (int id : targetIds)
				{
					if (player->id == id)
					{
						player->isTarget = true;
						uiInfo.targets.push_back(player);
					}

				}
			}


		}
}

//-------------------------------------------------------------//
void SingletonGlobalVars::FillScreenInfo(AddressesList& addressesList, ScreenData& screen, WorldToScreen& wts,
	std::shared_ptr<Me>& me)
{
	screen.fov[0] = *(float*)(addressesList.fovX);
	screen.fov[1] = *(float*)(addressesList.fovY);
	screen.res[0] = *(int*)(addressesList.screenResX);
	screen.res[1] = *(int*)(addressesList.screenResY);

	wts.fov[0] = *(float*)(addressesList.fovX);
	wts.fov[1] = *(float*)(addressesList.fovY);
	wts.me = me;
	wts.res[0] = *(int*)(addressesList.screenResX);
	wts.res[1] = *(int*)(addressesList.screenResY);
}

//-------------------------------------------------------------//
void SingletonGlobalVars::Aiming(bool varLock, bool aimKey, bool aimMod, bool isAim, bool chooseTargetOnce,
	std::shared_ptr<Ent>& entChosen, std::shared_ptr<Ent>& closestEntToCrosshairZerome)
{
	if (varLock)
	{
		if (IsKeyPressedOnce(aimKey, aimMod))
		{
			entChosen = closestEntToCrosshairZerome;
			isAim = !isAim;
		}
	}
	else
	{
		if (IsKeyPressed(aimKey, aimMod))
		{
			if (!chooseTargetOnce)
			{
				chooseTargetOnce = true;
				entChosen = closestEntToCrosshairZerome;
			}
			isAim = true;
		}
		else
		{
			chooseTargetOnce = false;
			isAim = false;
		}

	}

}