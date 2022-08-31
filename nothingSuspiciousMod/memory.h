#pragma once
#include <string>
#include <WinUser.h>

///////////////////////////////////////////////////////////////
// AddressesList - all necessary memory addresses
class AddressesList
{
	uintptr_t baseOJK = 0;
	uintptr_t baseMbii = 0;
public:
	uintptr_t playerOffset = 0x8D0;
	uintptr_t teamOffset = 0x2D10;
	uintptr_t enemyClassOffset = teamOffset;
	uintptr_t colorOffset = teamOffset;
	uintptr_t ammoOffset = teamOffset;

	uintptr_t consoleStart;
	uintptr_t consoleSize;
	uintptr_t ismenuOpen;
	uintptr_t trueId;
	uintptr_t entBase;
	uintptr_t myRawPitch;
	uintptr_t myRawYaw;
	uintptr_t myShield;
	uintptr_t myWristBlaster;
	uintptr_t IsAltFire;
	uintptr_t IsScope;
	uintptr_t isOnFire;
	uintptr_t ammo;
	uintptr_t maxPlayers;
	uintptr_t team;
	uintptr_t enClass;
	uintptr_t color1;
	uintptr_t color2;
	uintptr_t totalAmmo;
	uintptr_t clipAmmo;
	uintptr_t myNameAdd;
	uintptr_t weaponOrigin;
	uintptr_t ping;
	uintptr_t TraceLine;
	uintptr_t vehiclesEntities;
	uintptr_t mbiiSurfaceZeroNotInMatch;

	//actions
	uintptr_t w;
	uintptr_t s;
	uintptr_t a;
	uintptr_t d;
	uintptr_t att;
	uintptr_t speed;
	uintptr_t button12;
	uintptr_t altAttack; //maain
	uintptr_t moveDown;
	uintptr_t jump;
	uintptr_t crouch;
	uintptr_t use;
	uintptr_t inMatch;
	uintptr_t screenResX;
	uintptr_t screenResY;
	uintptr_t fovX;
	uintptr_t fovY;
	uintptr_t cameraPos;
	uintptr_t serverIp;
	uintptr_t lastAttacker;

	AddressesList(uintptr_t baseOJK, uintptr_t baseMbii);
};