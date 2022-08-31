#pragma once
#include <string>
#include <TlHelp32.h>
#include <WinUser.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <shlobj.h>
#include <objbase.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include <chrono>

#include <filesystem>

#include "hook.h"
#include "ent.h"
#include "memory.h"
#include "actions.h"

#pragma comment(lib,"Shell32")
#pragma comment(lib,"Ole32")

///////////////////////////////////////////////////////////////
// struct GameTimer
class GameTimer
{
    std::chrono::time_point<std::chrono::steady_clock> end, start;
    bool toggleTimer = false;
public:
    float period = 1000;
    float elapsedTime;

    void Start();
    void Reset();
};

class ExecuteCommand
{
    bool isExecingOnceAuto = false;
    GameTimer coolDown;
    std::string prevMessage;
public:
    bool isExecingOnce = false;
    std::string execPath;
    HWND mbiiHandle;
    void ExecCmd(std::string say, float coolDownPeriod);
    void ExecOneTime(std::string say);
};

///////////////////////////////////////////////////////////////
// struct ModuleAddresses - stores noduel base addresses
struct ModuleAddresses
{
    uintptr_t mbiiModuleBase;
    HANDLE mbiiHandle;
    uintptr_t ojkBase;
    uintptr_t openGLBase;
    uintptr_t kernelBase;
    uintptr_t rdVanilla;
    uintptr_t mbiiBase;

};

///////////////////////////////////////////////////////////////
// struct KeyModKey - stores pressed keys info
struct KeyModKey {
    int key;
    int modkey;
};



enum configCommands 
{
	CFG_AIM,
    CFG_AIM_NOPRED,
    CFG_TK,
    CFG_EXIT,
    CFG_THROUGHWALLS,
};

enum commandsList
{
    CMD_TARGET,
};


typedef char* (__cdecl* _Print)(const char* sFormat, ...);