#pragma once

#include <memory>
#include <vector>
#include "ent.h"
#include "actions.h"
#include "utils.h"
#include "GLDraw.h"
#include <algorithm>

///////////////////////////////////////////////////////////////
// struct Cplane_t - plane collision info
struct Cplane_t
{
	vec3_t	normal;
	float	dist;
	byte	type;			// for fast side tests: 0,1,2 = axial, 3 = nonaxial
	byte	signbits;		// signx + (signy<<1) + (signz<<2), used as lookup during collision
	byte	pad[2];
};

///////////////////////////////////////////////////////////////
// struct Trace_t - struct which returns collision info
struct Trace_t
{
	byte		allsolid;	// if true, plane is not valid
	byte		startsolid;	// if true, the initial point was in a solid area
	short		entityNum;	// entity the contacted sirface is a part of

	float		fraction;	// time completed, 1.0 = didn't hit anything
	vec3_t		endpos;		// final position
	Cplane_t	plane;		// surface normal at impact, transformed to world space
	int			surfaceFlags;	// surface hit
	int			contents;	// contents on other side of surface 

};


enum ProjectileVelocities
{
	VEL_PROJ = 10500,
	VEL_INSTANT = 3000000,
	VEL_PISTOL = 6325,
	VEL_E11_SEC = 4600,
	VEL_BOW = 7900,
	VEL_CR = 4000,
	VEL_DLT_PRIMARY = 5350,
	VEL_PLX = 1610,
	VEL_T21_SEC = 5600,
	VEL_SBD = 4900,
};



typedef void* (__cdecl* _TraceLine)(Trace_t* trace, const vec3_t mins, const vec3_t maxs,
	const vec3_t start, const vec3_t end,
	int skip, int brushmask, int setToZero);

///////////////////////////////////////////////////////////////
// class Aim - calculates everything for targets and does anything aim related
class Aim
{
public:
	Aim(std::shared_ptr<Me>& _me, std::vector<std::shared_ptr<Ent>>& _entsAlive, _TraceLine& __TraceLine,
		std::vector<std::shared_ptr<Ent>>& _entsAllies, std::vector<std::shared_ptr<Ent>>& _entsEnemies, ScreenData& _screen,
		WorldToScreen& _worldToScreen, std::unordered_map <int, int>& _varsMap)
	{
		meAim = _me;
		entsAliveAim = _entsAlive;
		TraceLine = __TraceLine;
		entsAlliesAim = _entsAllies;
		entsEnemiesAim = _entsEnemies;
		screen = _screen;
		worldToScreen = _worldToScreen;
		varsMap = _varsMap;
	}
	virtual ~Aim() = default;

public:


	//anti afk
	void AntiAfk(Actions& actions, const bool isAntiAfk, const bool isAfk);


	//anti-knockback
	void CrouchWhenAttacked(Actions& actions);


	//find closest to crosshair
	std::shared_ptr<Ent> FindClosestToCrosshair(std::vector<std::shared_ptr<Ent>>& entsSelected);


	//choose target
	std::shared_ptr<Ent> ChooseTarget(std::shared_ptr<Ent> &chosenTarget, const bool &isTk, bool& isAim);


	//lock on
	bool AimLock(std::shared_ptr<Ent>& enemy, int &iterations);


	//convert 3D into 2D screen coordinates
	void GetScreenCoordinates(std::vector<std::shared_ptr<Ent>>& entsSelected, const bool predictDraw);


	//calculate adjustments, angles, distances, visibility
	void EntCalculations(const int& slowFps, Actions& actions,
		const bool& targetThroughWalls, const bool& isPrediction);
private:
	bool IsOnGround(int stance) const;
	bool IsCrouched(int stance) const;
	float TestCrouch(std::shared_ptr<Ent> ent) const;
	void ModelHeightAdjustment(const std::shared_ptr<Me>& me, std::vector<std::shared_ptr<Ent>>& entsAlive);
	float ReturnAngle(std::shared_ptr<Ent>& enemy) const;
	float ReturnAngleFrom(std::shared_ptr<Ent>& enemy);
	void CalcDistAngles(std::vector<std::shared_ptr<Ent>>& entsAlive);
	void Adjustments(std::vector<std::shared_ptr<Ent>>& entsAlive);
	void isVisible(std::vector<std::shared_ptr<Ent>>& entsSelected, const int slowFps, const bool onOff);
	void CastVerticalRayDownFromEnemy(std::shared_ptr<Ent>& enemy);
	void Pblocks(std::shared_ptr<Ent>& enemy);
	float AngleBetweenEnemyX(float X, float Y) const;
	float ExtremeAngleAdjustment(float angleSetY) const;
	float Angle360EnemyToMe(std::shared_ptr<Ent>& enemy);
	void CalcAngleToSet(std::vector<std::shared_ptr<Ent>>& players, const bool isPrediction);

private:
	typedef struct _pblocksAngles {
		float swordPitch;
		float swordYaw;
	} pblocksAngles;
private:
	std::shared_ptr<Me> meAim;
	std::vector<std::shared_ptr<Ent>> entsAliveAim;
	std::vector<std::shared_ptr<Ent>> entsAlliesAim;
	std::vector<std::shared_ptr<Ent>> entsEnemiesAim;
	std::unordered_map <int, int> varsMap;
	_TraceLine TraceLine;
	ScreenData screen;
	WorldToScreen worldToScreen;
};