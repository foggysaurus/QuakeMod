#include "pch.h"

#include <memory>
#include <chrono>
#include <filesystem>
#include <regex>

#include "utils.h"
#include "ent.h"
#include "actions.h"


#define _CRT_SECURE_NO_WARNINGS
//int oldConsoleSize = 0;
//int idCheck = 0;
//std::string idStringCheck;
//std::string prevConsoleInput;
//
//HWND afkHD;
//HWND g_HWND;
//
//std::string consoleMsg;
//bool oldAfkWindow;
//bool oldActiveWindow;

void GameTimer::Start()
{
	if (!toggleTimer)
	{
		start = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> dur;
		dur = start - end;
		elapsedTime = dur.count() * 1000.0f;
		if (elapsedTime > period)
		{
			end = std::chrono::high_resolution_clock::now();
		}
	}
}
void GameTimer::Reset()
{
	toggleTimer = true;
	end = std::chrono::high_resolution_clock::now();
}

void ExecuteCommand::ExecCmd(std::string say, float coolDownPeriod)
{

	coolDown.Start();
	coolDown.period = coolDownPeriod;
	if (!isExecingOnceAuto && mbiiHandle)
	{
		
		prevMessage = say;
		std::ofstream ifile;
		ifile.open(execPath);
		ifile << say;
		ifile.close();
		std::cin.get();

		isExecingOnceAuto = true;
		SendMessage(mbiiHandle, WM_KEYDOWN, VK_LEFT, 0);
	}
	if (coolDown.elapsedTime > coolDown.period - 10)
	{

		isExecingOnceAuto = false;
	}
	//isExecingOnce = true;

}
void ExecuteCommand::ExecOneTime(std::string say)
{
	if (!isExecingOnce && mbiiHandle)
	{
		isExecingOnce = true;
		std::ofstream ifile;
		ifile.open(execPath);
		ifile << say;
		ifile.close();
		std::cin.get();
		SendMessage(mbiiHandle, WM_KEYDOWN, VK_LEFT, 0);
	}
}



