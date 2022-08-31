// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <filesystem>
#include <unordered_map>
#include <chrono>

#include "actions.h"
#include "hook.h"
#include "utils.h"
#include "aim.h"
#include "memory.h"
#include "ent.h"
#include "Wallhack.h"
#include "Globals.h"


typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);
twglSwapBuffers owglSwapBuffers;
SingletonGlobalVars& g = SingletonGlobalVars::get_instance();
//Globals *g;

BOOL __stdcall hkwglSwapBuffers(HDC hDc)
{
    WorldToScreen wts;

    std::string consoleLastLine;
    std::string leaderString;
    std::string newServerIp;

    std::shared_ptr<Me> me(new Me());
    std::shared_ptr<Ent> closestEntToCrosshairZerome(new Ent());
    std::shared_ptr<Ent> entAutoChosen(new Ent());

    std::vector<std::shared_ptr<Ent>> ents;
    std::vector<std::shared_ptr<Ent>> entsAlive;
    std::vector<std::shared_ptr<Ent>> entsAllies;
    std::vector<std::shared_ptr<Ent>> entsEnemies;
    std::vector<std::shared_ptr<Ent>> entsToAttack;

    ents.reserve(32);
    entsAlive.reserve(32);
    entsAllies.reserve(32);
    entsEnemies.reserve(32);

    _TraceLine TraceLine;

    const int maxPlayers = 33;
    int maxId = 0;

    if (!g.varsMap[VAR_LOCK])
        g.isAim = false;
    
    //clean up after closing cheat, restore hooked bytes
    if (g.bCloseGame)
    {
        glMatrixMode(GL_MODELVIEW);
        fclose(g.f);
        FreeConsole();
        Hook::ReturnBytes((BYTE*)GetProcAddress(GetModuleHandle("opengl32.dll"), "wglSwapBuffers"), (BYTE*)hkwglSwapBuffers, 5, (BYTE*)owglSwapBuffers);

        delete(g.modules);
        return owglSwapBuffers(hDc);
    }
    //things executed once on start (not every frame)
    if (!g.bEntryFlag)
    {
        g.bEntryFlag = true;

        g.ExecOnStartOnce(g.f, g.modules, g.hackConfigPath, g.execPath, g.playerNamesPath,
            g.executeCommand, g.commands, g.keyMap, g.varsMap, g.colorInfo, g.uiInfo, 
            g.rebuildFont, g.savedPlayerNames, g.afkHWND);
    }
    //close cheat
    if (g.IsKeyPressed(g.keyMap[CFG_EXIT].key, g.keyMap[CFG_EXIT].modkey))
    {
        g.bCloseGame = true;
    }
    //refresh UI and settings ctrl+r
    if (g.IsKeyPressed(0x52, 0xA2))
    {
        g.keyMap.clear();
        g.varsMap.clear();
        g.ReadCfg(g.hackConfigPath, g.keyMap, g.varsMap, g.colorInfo, g.uiInfo, g.rebuildFont);
    }
    //stop executing cheat if not in-game
    if (g.LockFromExecuting(g.modules, g.isAddressCheckLocked))
    {
        g.isAddressCheckLocked = false;
        return owglSwapBuffers(hDc);
    }

    //detect collapsed alt-windows
    g.AfkDetector(g.modules->mbiiModuleBase, g.afkHWND, g.myid, g.leaderId, g.isAfk);
    g.executeCommand.mbiiHandle = g.afkHWND;

    //set addresses
    AddressesList addressesList(g.modules->ojkBase, g.modules->mbiiBase);

    TraceLine = (_TraceLine)(addressesList.TraceLine);

    //reading last line of console in game
    consoleLastLine = g.ScanConsole(addressesList.consoleStart, addressesList.consoleSize);

    //checks when new round starts (newround true/false)
    g.NewRound(addressesList, g.newRound);

    //calculates frame rate
    g.newFrame++;
    g.FrameRate(g.startFrame, g.prevFrame, g.frameDuration, g.frames, g.slowFps);

    //----------------------------------------------LOGIC--------------------------------------------------//
    //set addresses for actions
    g.actions.SetActions(addressesList);

    //ignore walls when targeting
    if (!g.isAfk && g.IsKeyPressedOnce(g.keyMap[CFG_THROUGHWALLS].key, g.keyMap[CFG_THROUGHWALLS].modkey))
    {
        g.targetThroughWalls = !g.targetThroughWalls;
        g.uiInfo.isThroughWalls = g.targetThroughWalls;
    }

    //fills all arrays with all necessary info for aiming
    g.FillPlayerData(maxPlayers, addressesList, maxId, g.modules, ents, entsAlive, entsAllies, entsEnemies,
        me, g.dekasList, g.myName, g.newFrame, g.slowFps, g.allPlayerNames, g.myid, g.uiInfo);

    //reads player command from  console
    g.GetCommand(g.playerNamesPath, consoleLastLine, g.targetIds, g.friendIds, me, 
        g.savedPlayerNames, ents, g.modules, addressesList, g.isAfk);

    //find friends and set specific players - targets
    g.FindingFriendsAndTargets(g.uiInfo, g.targetIds, g.friendIds, ents);

    //autopunish and autoforgive on afk  alts
    if (g.varsMap[VAR_AUTOPUNISH] && g.isAfk)
        g.AutoPunish(me->lastAttacker, me, g.executeCommand);

    //---------------------stop executing hack if dead-----------------------------------------------------
    if (g.AmIdead(me))
    {
        if (!g.deadOnceLock)
        {
            g.executeCommand.isExecingOnce = false;
            g.deadOnceLock = true;
        }

        g.entChosen = nullptr;
        g.isAim = false;
        return owglSwapBuffers(hDc);
    }
    else
    {
        g.deadOnceLock = false;
    }

    //---------------------stop if dead----------------------------------------------------------

   //wallhack (fake sense)
    Wh16384 wh16384;
    wh16384.Wh(g.modules->mbiiBase, g.isAfk, g.varsMap[VAR_WALLHACK]);

    //get screen and FOV information
    g.FillScreenInfo(addressesList, g.screen, wts, me);

    Aim aim(me, entsAlive, TraceLine, entsAllies, entsEnemies, g.screen, wts, g.varsMap);



    if (!g.isAfk)
    {
        //toggle hitscan
        if (!g.varsMap[VAR_TOGGLETOHITSCAN])
        {
            g.isPrediction = !g.IsKeyPressed(g.keyMap[CFG_AIM_NOPRED].key, g.keyMap[CFG_AIM_NOPRED].modkey);
            g.uiInfo.isHitscan = g.isPrediction;
        }
        else
        {
            if (g.IsKeyPressedOnce(g.keyMap[CFG_AIM_NOPRED].key, g.keyMap[CFG_AIM_NOPRED].modkey))
                g.isPrediction = !g.isPrediction;
            g.uiInfo.isHitscan = g.isPrediction;
        }
    }
    //calculate stuff for ents
    aim.EntCalculations(g.slowFps, g.actions, g.targetThroughWalls, g.isPrediction);
    //just an anti-afk script
    aim.AntiAfk(g.actions, g.varsMap[VAR_ANTIAFK], g.isAfk);


    //----not afk------------------------------------------------------------------------------------------
    if (!g.isAfk)
    {
        //select ents if TK is not toggled

        if (g.IsKeyPressedOnce(g.keyMap[CFG_TK].key, g.keyMap[CFG_TK].modkey))
        {
            g.isTk = !g.isTk;
            g.uiInfo.isTK = g.isTk;
        }
        if (g.isTk)
            entsToAttack = entsAlive;
        else
            entsToAttack = entsEnemies;

        //convert 3D coordiantes into 2D screen coordinates
        aim.GetScreenCoordinates(entsAlive, false);

        //find closest enemy to crosshair
        closestEntToCrosshairZerome = aim.FindClosestToCrosshair(entsAlive);
        closestEntToCrosshairZerome->isClosestEnemy = true;

        //draw lines, boxes and captions 
        GL::DrawEverything(entsAlive, me, g.screen, g.varsMap, g.colorInfo, g.uiInfo, true, g.rebuildFont);
        
        g.Aiming(g.varsMap[VAR_LOCK], g.keyMap[CFG_TK].key, g.keyMap[CFG_TK].modkey, g.isAim, 
            g.chooseTargetOnce, g.entChosen,
            closestEntToCrosshairZerome);

        //choose enemy automatically
        entAutoChosen = aim.ChooseTarget(g.entChosen, g.isTk, g.isAim);

        //if aim and ent exists lock onto the target
        if (g.isAim && entAutoChosen)
        {
            aim.AimLock(entAutoChosen, g.varsMap[VAR_SMOOTHAIM]);
        }
    }

    //---------------------------------------------------endLogic----------------------------------------------------//
    //reset FPS counter
    if (g.newFrame > g.slowFps + 2)
    {
        g.newFrame = 0;
    }

    return owglSwapBuffers(hDc);
}

//-------------------------------------------------------------//
DWORD WINAPI MainThread(HMODULE thisDLLHmod)
{
    
    owglSwapBuffers = (twglSwapBuffers)GetProcAddress(GetModuleHandle("opengl32.dll"), "wglSwapBuffers");
    owglSwapBuffers = (twglSwapBuffers)Hook::TrampHook32((BYTE*)owglSwapBuffers, (BYTE*)hkwglSwapBuffers, 5);
    
    while (true)
    {
        Sleep(100);

        if (g.bCloseGame)
        {
            Sleep(1000);
            break;
        }
    }
    FreeLibraryAndExitThread(thisDLLHmod, 0);
    return 1;
}

//-------------------------------------------------------------//
BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        g.thisDLLHmod = hModule;
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)(MainThread), hModule, 0, nullptr));
    }

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

