#pragma once

#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <filesystem>
#include <unordered_map>
#include <chrono>

#include "ent.h"
#include "utils.h"
#include "aim.h"

class SingletonGlobalVars
{
protected:
	SingletonGlobalVars() = default;
public:

	HMODULE thisDLLHmod;
	Actions actions;
	ModuleAddresses* modules = new ModuleAddresses();
	GameTimer timerAdad;
	GameTimer WH;

	ColorInfo colorInfo;
	UIInfo uiInfo;
	ExecuteCommand timerSpammer;
	ExecuteCommand executeCommand;
	uintptr_t mbiiBaseAddress;
	std::unordered_map <int, KeyModKey> keyMap;
	std::unordered_map <int, int> varsMap;
	std::unordered_map <int, int> commandsMap;
	std::shared_ptr<Ent> entChosen;
	std::vector<PlayerNames> allPlayerNames;
	std::vector<std::string> savedPlayerNames;
	std::vector<int> dekasList;
	std::vector<float> frames;
	std::vector<int> targetIds;
	std::vector<int> friendIds;
	std::string myName;
	std::string execPath;
	std::string serverIp;
	std::wstring hackConfigPath;
	std::wstring playerNamesPath;
	std::chrono::time_point<std::chrono::steady_clock> startFrame, prevFrame;
	std::chrono::duration<float> frameDuration = std::chrono::seconds(1);

	ScreenData screen;
	HWND afkHWND;

	int newFrame = 0;
	int slowFps = 60;
	int myid;
	int leaderId = -1;


	FILE* f = new FILE;

	std::vector<std::string> commands;

	bool bCloseGame;
	bool newRound;
	bool bEntryFlag;
	bool inputOnce;
	bool isTk;
	bool isAfk;
	bool isAim;
	bool oldWindow;
	bool detachSwapBuffers;
	bool rebuildFont = false;
	bool targetThroughWalls;
	bool chooseTargetOnce;
	bool isAddressCheckLocked = false;
	bool writeNamesOnce;
	bool isPrediction = true;
	bool deadOnceLock = false;

	static SingletonGlobalVars& get_instance()
	{
		static SingletonGlobalVars instance;
		return instance;
	}
private:
    bool oldAfkWindow;
    bool oldActiveWindow;
    int idCheck = 0;
    int oldConsoleSize = 0;
    std::string prevConsoleInput;
    HWND g_HWND = NULL;
    std::string consoleMsg;
    std::string idStringCheck;

public:
    //ExecOnStartOnce - execute commands only upon hack launch
    void ExecOnStartOnce(FILE* f, ModuleAddresses* modules, std::wstring& hackConfigPath,
        std::string& execPath, std::wstring& playerNamesPath, ExecuteCommand& executeCommand,
        std::vector<std::string>& commands, std::unordered_map <int, KeyModKey>& keyMap,
        std::unordered_map <int, int>& varsMap, ColorInfo& colorInfo,
        UIInfo& uiInfo, bool& rebuildFont, std::vector<std::string>& savedPlayerNames, HWND& afkHandle);

    // IsKeyPressedOnce - checks if key was pressed once
    bool IsKeyPressedOnce(int key, int modKey);

    // IsKeyPressed - checks if key is being held
    bool IsKeyPressed(int key, int modKey);

    // Read cfg - read config file
    void ReadCfg(std::wstring hackPath, std::unordered_map <int, KeyModKey>& keyMap,
        std::unordered_map <int, int>& varsMap, ColorInfo& colorInfo, UIInfo& uinfo, bool& rebuildFont);

    //LockFromExecuting - code will not proceed after this point if you are not in game
    bool LockFromExecuting(ModuleAddresses* modules, bool& isAddressCheckLocked);

    // AfkDetector - detects AFK windows
    bool AfkDetector(int procId, HWND& afkHWND, int myId, int& leaderId, bool& isAfk);

    // ScanConsole - scans game console returns the last change comapred to previous frame
    std::string ScanConsole(uintptr_t& addressCons, uintptr_t& addressOffs);

    //NewRound - checks if new round started
    void NewRound(AddressesList& addressesList, bool& newRound);
    // FrameRate - checks framerate
    void FrameRate(std::chrono::time_point<std::chrono::steady_clock>& startFrame,
        std::chrono::time_point<std::chrono::steady_clock>& prevFrame,
        std::chrono::duration<float>& frameDuration, std::vector<float>& frames, int& slowFps);

    //FillPlayerData - fill everything player related at once
    void FillPlayerData(const int& maxPlayers, AddressesList& addressesList, int& maxId, ModuleAddresses* modules,
        std::vector<std::shared_ptr<Ent>>& ents, std::vector<std::shared_ptr<Ent>>& entsAlive, 
        std::vector<std::shared_ptr<Ent>>& entsAllies, std::vector<std::shared_ptr<Ent>>& entsEnemies, 
        std::shared_ptr<Me>& me, std::vector<int>& dekasList, std::string& myName, int newFrame, int slowFps, 
        std::vector<PlayerNames>& allPlayerNames, int myid, UIInfo& uiInfo);


    //GetCOmmand - read console for user commands
    bool GetCommand(std::wstring& playerNamesPath, std::string lastLineOfConsole,
        std::vector<int>& targetIds, std::vector<int>& friendIds,
        std::shared_ptr<Me> me, std::vector<std::string>& savedPlayerNames,
        std::vector<std::shared_ptr<Ent>>& ents, ModuleAddresses* modules,
        AddressesList& addressesList, bool isAfk);

    // FindingFriendsAndTargets - find user selected friends and targets
    void FindingFriendsAndTargets(UIInfo& uiInfo, std::vector<int>& targetIds, std::vector<int>& friendIds,
        std::vector<std::shared_ptr<Ent>>& ents);

    //AutoPunish - !p teamkillers or !f allies
    void AutoPunish(std::shared_ptr<Ent>& ent, std::shared_ptr<Me>& me, ExecuteCommand& executeCommand);

    //AmiDead - check if i am dead
    bool AmIdead(std::shared_ptr<Me>& me);

    //FillScreenInfo - all UI related information is filled here
    void FillScreenInfo(AddressesList& addressesList, ScreenData& screen, WorldToScreen& wts, 
        std::shared_ptr<Me>& me);

    // Aiming - all aim- related calculations are done here
    void Aiming(bool varLock, bool aimKey, bool aimMod, bool isAim, bool chooseTargetOnce,
        std::shared_ptr<Ent>& entChosen, std::shared_ptr<Ent>& closestEntToCrosshairZerome);
private:

    // CreateMapping - create mapping (for interprocess communication)
    void CreateMapping(std::string& buffer);

    // ReadFromMap - read map (for interprocess communication)
    bool ReadFromMap(std::string& result);

    // GetModId - finds process module memory address
    HMODULE GetModId(int procId, const char* modName);

    // SetAddresses - finds addresses of all relevent process modules
    void SetAddresses(ModuleAddresses* moduleAddresses);

    // Wexists - checks if wide file exists
    bool WExists(const std::wstring& name);

    // getDesctopPath - gets desktop path
    std::wstring GetDesktopPath();

    // GetCurrentDirectory - gets current directory
    std::string GetCurrentDirectory();

    // ReadBinds reads the hack config file
    void ReadBinds(std::string consoleLastLine);

    // FromStringTointSafe - converts from string to int safely
    int FromStringToIntSafe(std::string in, int base);

    // FromStringToFloatSafe - converts from string to float safely
    float FromStringToFloatSafe(std::string in);

    // FillSavedNames - reads name list once and saves into a vector for performance
    void FillSavedNames(std::vector<std::string>& savedPlayerNames, std::wstring hackConfigPath);

    // CheckToAppendFast - checks if a line needs to be appended in the hack config file (checks for duplicates)
    bool CheckToAppendFast(std::vector<std::string>& savedPlayerNames, std::wstring hackConfigPath,
        std::string lineToCheck);

    // CheckToAppend - (checks for duplicates)
    bool CheckToAppend(std::wstring hackConfigPath, std::string lineToCheck, int defaultLevel);
    bool CheckToAppend(std::wstring hackConfigPath, std::string lineToCheck, double defaultLevel);
    bool CheckToAppend(std::wstring hackConfigPath, std::string lineToCheck, std::string extra);

    // WriteCfg - write once into hack config file
    void WriteCfg(std::wstring hackConfigPath, std::vector<std::string>& commands);

    //GetMyName - get my name from a different location from others
    std::string GetMyName(uintptr_t address);

    //GetName - gets my name
    std::string GetName(std::shared_ptr<Ent> player, ModuleAddresses* modules, AddressesList& addressesList);

    //GetShortName - removes color codes from names
    std::string GetShortName(std::string& name);

    //ReturnHp - gets player HP
    void ReturnHp(std::vector<std::shared_ptr<Ent>>& entsAlive);

    //FIll names once per second (for performance)
    void FillNames(std::vector<std::shared_ptr<Ent>>& ents, ModuleAddresses* modules,
        std::vector<PlayerNames>& allPlayerNames, AddressesList& addressesList, int& newFrame, int& slowFps);

    //FindDekas - find droidekas (are treated as NPCs and thus are separate form the rest of the players)
    void FindDekas(AddressesList& addressesList, std::vector<int>& dekasList, int& newFrame, int& slowFps);

    //AddDekas add them into the player list
    void AddDekas(std::vector<int>& dekasList, AddressesList& addressesList, std::vector<std::shared_ptr<Ent>>& entsAlive, std::shared_ptr<Me>& me);

    //FillArrays - fill vectors with players
    void FillArrays(std::vector<std::shared_ptr<Ent>>& ents, std::vector<std::shared_ptr<Ent>>& entsAlive, std::vector<std::shared_ptr<Ent>>& entsAllies, std::vector<std::shared_ptr<Ent>>& entsEnemies, std::shared_ptr<Me>& me,
        AddressesList& addressesList, ModuleAddresses* modules, int maxId);

    // FillMe - get my stats
    void FillMe(std::vector<std::shared_ptr<Ent>>& ents, std::shared_ptr<Me>& me, AddressesList& addressesList, ModuleAddresses* modules, std::string& myName);

    //FindMaxId -  finds a player with maxId
    void FindMaxId(const int maxPlayers, AddressesList& addressesList, int maxId);
};
