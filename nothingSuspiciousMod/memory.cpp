#include "pch.h"
#include "memory.h"

AddressesList::AddressesList(uintptr_t baseOJK, uintptr_t baseMbii)
{
	consoleStart = baseMbii + 0x1166610;
	consoleSize = baseMbii + 0x1176610;
	ismenuOpen = baseMbii + 0x62D2E0;
	trueId = baseMbii + 0xB2A960;
	myRawPitch = baseMbii + 0x644A48; //change Pitch
	myRawYaw = baseMbii + 0x644A4C;
	myShield = baseMbii + 0x63DCD8;
	myWristBlaster = baseMbii + 0x63DD00;
	IsAltFire = baseMbii + 0x63DC54;
	IsScope = baseMbii + 0x63E110; //4 scope
	isOnFire = baseMbii + 0x63DCD0; //2 on fire
	ammo = baseMbii + 0x63DCEC;
	maxPlayers = baseMbii + 0x2CF5B0;
	myNameAdd = baseMbii + 0x1D09B8;
	ping = baseMbii + 0x64EBB4;
	w = baseMbii + 0x62D060;
	s = baseMbii + 0x62D078;
	a = baseMbii + 0x62D0C0;
	d = baseMbii + 0x62D0D8;
	att = baseMbii + 0x62D150;
	speed = baseMbii + 0x62D108;
	button12 = baseMbii + 0x62D270;
	altAttack = baseMbii + 0x62D1F8; //maain
	moveDown = baseMbii + 0x62D138;
	jump = baseMbii + 0x62D120;
	crouch = baseMbii + 0x62D138;
	use = baseMbii + 0x62D1C8;
	mbiiSurfaceZeroNotInMatch = baseMbii + 0x63DC34;
	inMatch = baseMbii + 0x647010;//baseMbii + 0x63DD14; //    uintptr_t inMatch = 0x647010;
	screenResX = baseMbii + 0x1387E4;
	screenResY = baseMbii + 0x138830;
	cameraPos = baseMbii + 0x114B15C; //0xE514B8
	serverIp = baseMbii + 0x1EE9E0;
	lastAttacker = baseMbii + 0x63DD1C;


	entBase = baseOJK + 0x4F768E0;
	fovX = entBase + 0x234D94;
	fovY = fovX + 4;
	vehiclesEntities = entBase + 4;
	TraceLine = entBase - 0x4F09DA0;
	weaponOrigin = entBase - 0x4E3665C;
	team = entBase + 0x28EFB0;
	enClass = entBase + 0x28F2B0;
	color1 = entBase + 0x28EFE0;
	color2 = color1 + 4;
	totalAmmo = entBase + 0x290234; //ojk + 2d10
	clipAmmo = totalAmmo + 0x4;
};