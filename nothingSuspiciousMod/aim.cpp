#include "pch.h"
#include "ent.h"
#include <string>
#include <WinUser.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <shlobj.h>
#include <objbase.h>
#include <memory>
#include <vector>
#include <chrono>
#include "ent.h"
#include "aim.h"
#include "memory.h"

constexpr float kModelHeight = 64.118f;
constexpr float kHeadAdjustment = 1.9f;
constexpr float kCrouchAdjustment = 3.0f;
constexpr float kOnGround = 2.0f;
constexpr float kCrouchHeightAdjustment = -35.0f;
constexpr float kDegreesToRad = PI / 180.0f;
constexpr float kRadToDegrees = 180.0f/ PI;

constexpr float kDefaultDistAdjustment = 920.0;
constexpr float kDistanceMultiplier = 900.0f;

GameTimer jumpTimer;
GameTimer afkTimer;
ExecuteCommand antiAfkCommand;

std::vector<float> memCoordX;
std::vector<float> memCoordY;
int iterator = 0;

bool Aim::IsOnGround(int stance) const 
{
	bool isFallen = false;
	if (stance < 50
		|| stance == 1234
		|| stance == 1257
		|| stance == 1256
		|| stance == 1258)
	{
		isFallen = true;
	}

	return isFallen;
}

//-------------------------------------------------------------//
bool Aim::IsCrouched(int stance) const
{
	return stance == 1018
		|| stance == 1019
		|| stance == 1020
		|| stance == 1182
		|| stance == 1184
		|| stance == 1185
		|| stance == 1204
		|| stance == 1025
		|| stance == 1178
		|| stance == 1544
		|| stance == 1184
		|| stance == 881
		|| stance == 883
		|| stance == 1185
		|| stance == 1182;
}

//-------------------------------------------------------------//
float Aim::TestCrouch(std::shared_ptr<Ent> ent) const
{
	float crouch = 0;


	if ((ent->stance == 1537
		|| ent->stance == 899
		|| ent->stance == 900
		|| ent->stance == 901
		|| ent->stance == 902
		|| ent->stance == 1556
		|| ent->stance == 911
		|| ent->stance == 1557
		|| ent->stance == 902
		|| ent->stance == 901
		|| ent->stance == 907
		|| ent->stance == 908
		|| ent->stance == 909
		|| ent->stance == 910
		|| ent->stance == 1255
		|| ent->stance == 1254
		|| ent->stance2 == 1582
		|| ent->stance2 == 1552
		|| ent->stance2 == 1400
		)
		&& ent->anglesDistances.distanceAbs != 0)
	{
		crouch = kCrouchHeightAdjustment;
	}
	return crouch;
}

//-------------------------------------------------------------//
void Aim::AntiAfk(Actions& actions, const bool isAntiAfk, const bool isAfk)
{
	if (isAfk && isAntiAfk)
	{
		afkTimer.period = 29000;
		afkTimer.Start();
		if (afkTimer.elapsedTime > afkTimer.period - 60 
			&& afkTimer.elapsedTime < afkTimer.period - 30)
		{
			actions.Stop(actions.crouch);
		}
		if (afkTimer.elapsedTime > afkTimer.period - 30)
		{
			actions.Do(actions.crouch);
		}
	}

}

//-------------------------------------------------------------//
void Aim::CrouchWhenAttacked(Actions &actions)
{
	//don't crouch if you're wook
	if (varsMap[VAR_ANTIKNOCKBACK])
		for (std::shared_ptr<Ent> player : entsAliveAim)
		{
			if (meAim->entClass != Classes::CL_WOOK && meAim->entClass != Classes::CL_SBD)
			{
				if (TestCrouch(player) && player->anglesDistances.distanceAbs < 110)
				{
					actions.Do(actions.crouch);
					break;
				}
				else
				{
					actions.Stop(actions.crouch);
				}
				if (player->stance == 1713 
					&& player->anglesDistances.distanceAbs < 65 
					&& meAim->surface != 1023)
				{
					actions.Do(actions.jump);
					break;
				}
				else
				{
					actions.Stop(actions.jump);
				}
			}
			else
			{
				if ((player->stance2 == 1400 || player->stance2 == 1552) 
					&& player->anglesDistances.distanceAbs < 110)
				{
					actions.Do(actions.crouch);
					break;
				}
				else
				{
					actions.Stop(actions.crouch);
				}
			}
		}

}

//-------------------------------------------------------------//
void Aim::ModelHeightAdjustment(const std::shared_ptr<Me>& me, std::vector<std::shared_ptr<Ent>>& entsAlive)
{
	float heightTestMe = 0;
	float heightTestEnemy = 0;
	float modelHeightMine = 0;


	//me
	if (me->myCharScale == 0)
	{
		me->myCharScale = 100;
	} //myChar =100
	modelHeightMine = kModelHeight * me->myCharScale / 100.0f;
	me->realModelHeight = modelHeightMine;


	me->pos[2] += (float)(modelHeightMine / kHeadAdjustment);

	if (me->moveDown)
		me->pos[2] -= modelHeightMine / kCrouchAdjustment;
	//me 

	//------------

	//enemy
	for (std::shared_ptr<Ent> &player : entsAlive)
	{
		float modelHeightReal = 0;
		if (player->charScale == 0)
		{
			player->charScale = 100;
		}
		modelHeightReal = kModelHeight * player->charScale / 100.0f;
		player->realModelHeight = modelHeightReal;
		player->pos[2] += modelHeightReal / 4.3;
		//player->pos[2] += modelHeight * player->charScale / 100.0f - modelHeight;
		if (IsCrouched(player->stance) || player->stance2 == 967)
		{
			player->pos[2] -= modelHeightReal / kCrouchAdjustment;
		}
		if (IsOnGround(player->stance))
		{
			player->pos[2] -= modelHeightReal / kOnGround;
		}
		//player->pos[2] += 30;
	}


}

//-------------------------------------------------------------//
float Aim::ReturnAngle(std::shared_ptr<Ent>& enemy) const
{
	float finalAngle = 0; //-500 30 200
	vec2_t oneVec;
	vec3_t enemyMe;
	vec3_t oneVecFinal;
	float top;
	float bottom;
	oneVec[0] = 1;
	oneVec[1] = (float)tan(meAim->yaw * kRadToDegrees);

	if (meAim->yaw > 90 || meAim->yaw < -90)
	{
		oneVec[0] = -1;
		oneVec[1] = (float)-tan(meAim->yaw * kRadToDegrees);
	}
	float oneVecLength = sqrt(1 + oneVec[1] * oneVec[1]);

	//VecCopy(enemyMe, enemy->pos);
	//VecSub(enemyMe, me->pos);

	enemyMe[0] = enemy->pos[0] - meAim->pos[0];
	enemyMe[1] = enemy->pos[2] - meAim->pos[2];
	enemyMe[2] = enemy->pos[1] - meAim->pos[1];


	oneVecFinal[0] = oneVec[0];
	oneVecFinal[1] = (float)tan(-meAim->pitch * kRadToDegrees) * oneVecLength;
	oneVecFinal[2] = oneVec[1];

	top = VecMult(enemyMe, oneVecFinal);

	bottom = sqrt(enemyMe[0] * enemyMe[0] + enemyMe[1] * enemyMe[1] + enemyMe[2] * enemyMe[2]) * sqrt(oneVecFinal[0] * oneVecFinal[0] + oneVecFinal[1] * oneVecFinal[1] + oneVecFinal[2] * oneVecFinal[2]);

	finalAngle = (float)(acos(top / bottom) * kRadToDegrees);

	//std::cout << finalAngle << std::endl;

	return finalAngle;
}

//-------------------------------------------------------------//
float Aim::ReturnAngleFrom(std::shared_ptr<Ent>& enemy)
{
	float finalAngle = 0; //-500 30 200
	vec2_t oneVec;
	vec3_t enemyMe;
	vec3_t oneVecFinal;
	float top;
	float bottom;
	oneVec[0] = 1;
	oneVec[1] = (float)tan(enemy->enYaw * kDegreesToRad);

	if (enemy->enYaw > 90 || enemy->enYaw < -90)
	{
		oneVec[0] = -1;
		oneVec[1] = (float)-tan(enemy->enYaw * kDegreesToRad);
	}
	float oneVecLength = sqrt(1 + oneVec[1] * oneVec[1]);

	enemyMe[0] = meAim->pos[0] - enemy->pos[0];
	enemyMe[1] = meAim->pos[2] - enemy->pos[2];
	enemyMe[2] = meAim->pos[1] - enemy->pos[1];


	oneVecFinal[0] = oneVec[0];
	oneVecFinal[1] = (float)tan(-enemy->enPitch * kDegreesToRad) * oneVecLength;
	oneVecFinal[2] = oneVec[1];

	top = VecMult(enemyMe, oneVecFinal);

	bottom = sqrt(enemyMe[0] * enemyMe[0] + enemyMe[1] 
		* enemyMe[1] + enemyMe[2] * enemyMe[2]) 
		* sqrt(oneVecFinal[0] * oneVecFinal[0] 
		+ oneVecFinal[1] * oneVecFinal[1] 
		+ oneVecFinal[2] * oneVecFinal[2]);

	finalAngle = (float)(acos(top / bottom) * kRadToDegrees);

	return finalAngle;
} 

//-------------------------------------------------------------//
void Aim::CalcDistAngles(std::vector<std::shared_ptr<Ent>>& entsAlive)
{
	for (std::shared_ptr<Ent> player : entsAlive)
	{
		player->anglesDistances.distanceXY = VecDistance2(meAim->pos, player->pos);
		player->anglesDistances.distanceAbs = VecDistance(meAim->pos, player->pos);
		player->anglesDistances.angleTo = ReturnAngle(player);
		player->anglesDistances.angleFrom = ReturnAngleFrom(player);
	}
}

//-------------------------------------------------------------//
float Aim::AngleBetweenEnemyX(float X, float Y) const
{
	float angle;

	if (((X < 0) && (Y > 0)) || ((X < 0) && (Y < 0)))
	{
		angle = atan(Y / X);
		angle += (float)PI;
	}
	else if ((X > 0) && (Y < 0))
		angle = (float)(2 * PI + atan(Y / X));
	else
		angle = (float)atan(Y / X);

	float finalAngle = (float)((kRadToDegrees) * angle);
	return finalAngle;
}

//-------------------------------------------------------------//
void Aim::Adjustments(std::vector<std::shared_ptr<Ent>>& entsAlive)
{
	for (std::shared_ptr<Ent>& target : entsAlive)
	{
		std::shared_ptr<WeaponInfo> weaponsInfo (new WeaponInfo());
		weaponsInfo->distAdjust = target->anglesDistances.distanceAbs / kDistanceMultiplier;
		weaponsInfo->shoulderAdj = 0;
		weaponsInfo->projvelAdj = 0;
		auto myWeapon = meAim->weapon;

		const float kPingAdjustment = -0.0011f;

		const int kProjShoulder = -50;
		const int kWristShoulder = 150;
		const int kWestarsShoulder = 350;
		const int kPistolsShoulder = -80;
		const int kCRShoulder = 300;
		const int kSBDShoulder = 480;
		const int kWestarM5Shoulder = 10;
		const int kWestarM5ScopedShoulder = -80;
		const int kDLTShoulder = 280;
		const int kT21Shoulder = 100;
		const int kE11Shoulder = 310;
		const int kE11PrimaryShoulder = 200;
		const int kEE3Shoulder = 100;

		const int kWristBlasterOn = 128;
		const int kDLTSecondaryFire = 66048;


		switch (myWeapon)
		{
		case Weapons::WP_PROJ: weaponsInfo->projvelAdj = kDefaultDistAdjustment / VEL_PROJ; break;
		case Weapons::WP_MELEE: weaponsInfo->projvelAdj = kDefaultDistAdjustment / VEL_INSTANT; break;
		case Weapons::WP_LS: weaponsInfo->projvelAdj = kDefaultDistAdjustment / VEL_INSTANT; break;
		case Weapons::WP_PISTOL: weaponsInfo->projvelAdj = kDefaultDistAdjustment / VEL_PISTOL; break;
		case Weapons::WP_E11: weaponsInfo->projvelAdj = kDefaultDistAdjustment / VEL_E11_SEC; break;
		case Weapons::WP_DISR: weaponsInfo->projvelAdj = kDefaultDistAdjustment / VEL_INSTANT; break;
		case Weapons::WP_BOW: weaponsInfo->projvelAdj = kDefaultDistAdjustment / VEL_BOW; break;
		case Weapons::WP_CR: weaponsInfo->projvelAdj = kDefaultDistAdjustment / VEL_CR; break;
		case Weapons::WP_CPISTOL: weaponsInfo->projvelAdj = kDefaultDistAdjustment / VEL_PISTOL; break;
		case Weapons::WP_DLT: weaponsInfo->projvelAdj = kDefaultDistAdjustment / VEL_DLT_PRIMARY; break;
		case Weapons::WP_PLX: weaponsInfo->projvelAdj = kDefaultDistAdjustment / VEL_PLX; break;
		case Weapons::WP_T21: weaponsInfo->projvelAdj = kDefaultDistAdjustment / VEL_T21_SEC; break;
		case Weapons::WP_SBD: weaponsInfo->projvelAdj = (float)(kDefaultDistAdjustment / VEL_SBD); break;
		case Weapons::WP_WPISTOL: weaponsInfo->projvelAdj = kDefaultDistAdjustment / VEL_PISTOL; break;
		default:  weaponsInfo->projvelAdj = kDefaultDistAdjustment / VEL_PISTOL; break;
		}

		if (myWeapon == Weapons::WP_PROJ)
		{
			weaponsInfo->shoulderAdj = (kProjShoulder / target->anglesDistances.distanceAbs);
		} //proj
		else if (myWeapon == Weapons::WP_MELEE && meAim->entClass == Classes::CL_MANDA 
			&& meAim->WristBlaster == kWristBlasterOn)
		{
			weaponsInfo->projvelAdj = kDefaultDistAdjustment / VEL_PISTOL;
			weaponsInfo->shoulderAdj = (kWristShoulder / target->anglesDistances.distanceAbs);
		} //wrist mando
		else if (myWeapon == Weapons::WP_PISTOL && meAim->entClass != Classes::CL_DEKA
			|| myWeapon == Weapons::WP_CPISTOL)
		{
			weaponsInfo->shoulderAdj = (kWestarsShoulder / target->anglesDistances.distanceAbs);
		} //pistols
		else if (myWeapon == Weapons::WP_WPISTOL)
		{
			weaponsInfo->shoulderAdj = (kPistolsShoulder / target->anglesDistances.distanceAbs);
		} //westar
		else if (myWeapon == Weapons::WP_CR)
		{
			weaponsInfo->shoulderAdj = (kCRShoulder / target->anglesDistances.distanceAbs);
			if (meAim->amAttacking)
				weaponsInfo->projvelAdj *= 0.87f;
		} //clone rifle
		else if (myWeapon == Weapons::WP_DISR || myWeapon == Weapons::WP_LS
			|| (myWeapon == Weapons::WP_MELEE && meAim->entClass != Classes::CL_MANDA))
		{
			weaponsInfo->distAdjust = 0;
			weaponsInfo->shoulderAdj = 0;
			weaponsInfo->projvelAdj = 0;

		} //disruptor
		else if (myWeapon == Weapons::WP_SBD)
		{
			weaponsInfo->shoulderAdj = (kSBDShoulder / target->anglesDistances.distanceAbs);
		} //sbd
		else if (myWeapon == Weapons::WP_DLT && meAim->entClass == Classes::CL_ARC)
		{
			weaponsInfo->projvelAdj *= 1.1f;
			weaponsInfo->shoulderAdj = (kWestarM5Shoulder / target->anglesDistances.distanceAbs);
			if (meAim->IsScope)
			{
				weaponsInfo->projvelAdj *= 0.47f;
				weaponsInfo->shoulderAdj = (kWestarM5ScopedShoulder / target->anglesDistances.distanceAbs);
			}
		} //sbd
		else if (myWeapon == Weapons::WP_DLT && (meAim->entClass == Classes::CL_ET 
			|| meAim->entClass == Classes::CL_BH))
		{
			weaponsInfo->shoulderAdj = (kDLTShoulder / target->anglesDistances.distanceAbs); //280
			if (meAim->IsAltFire == kDLTSecondaryFire) //firing with altfire
				weaponsInfo->projvelAdj *= 0.66f;
		} //a280 commander
		else if (myWeapon == Weapons::WP_T21)
		{
			weaponsInfo->shoulderAdj = (kT21Shoulder / target->anglesDistances.distanceAbs);
		} //t21
		else if (myWeapon == Weapons::WP_E11)
		{
			weaponsInfo->shoulderAdj = (kE11Shoulder / target->anglesDistances.distanceAbs);
			if (meAim->amAttacking)
			{
				weaponsInfo->projvelAdj *= 0.7f;
				weaponsInfo->shoulderAdj = (kE11PrimaryShoulder / target->anglesDistances.distanceAbs);
			}
		}  //e11
		else if (myWeapon == Weapons::WP_DLT && meAim->IsScope && meAim->entClass == Classes::CL_MANDA)
		{
			weaponsInfo->projvelAdj *= 0.5f;
		} //scope manda
		else if (myWeapon == Weapons::WP_DLT && meAim->entClass == Classes::CL_MANDA) //manda
		{
			weaponsInfo->projvelAdj *= 0.85f;
			weaponsInfo->shoulderAdj = (kEE3Shoulder / target->anglesDistances.distanceAbs);
		} //manda ee3
		else if (myWeapon == Weapons::WP_PISTOL && meAim->entClass == Classes::CL_DEKA)
		{
			weaponsInfo->projvelAdj *= 1.5f;
			weaponsInfo->shoulderAdj = (0 / target->anglesDistances.distanceAbs);
		} //deka


		float pingAfj = (float)kPingAdjustment * meAim->ping;
		float pingDistProjAdj = pingAfj - weaponsInfo->distAdjust * weaponsInfo->projvelAdj;

		//predicted enemy position in accordance with projectile velocity
		target->predPos[0] = target->pos[0] - target->speed[0] * pingDistProjAdj;
		target->predPos[1] = target->pos[1] - target->speed[1] * pingDistProjAdj;
		target->predPos[2] = target->pos[2] - target->speed[2] * pingDistProjAdj;

		VecCopy(target->posPing, target->pos);

		//enemy position for hitscan weapons (accounts for ping only)
		target->posPing[0] -= (pingAfj * target->speed[0]);
		target->posPing[1] -= (pingAfj * target->speed[1]);
		target->posPing[2] -= (pingAfj * target->speed[2]);

		if (!target->speed[0] && !target->speed[1] && (target->weapon == WP_SBD 
			|| target->weapon == WP_WPISTOL))
			weaponsInfo->shoulderAdj = 0;

		target->weaponInfo = weaponsInfo;
	}
}

//-------------------------------------------------------------//
void Aim::isVisible(std::vector<std::shared_ptr<Ent>>& entsSelected, const int slowFps, const bool onOff)
{
	if (!onOff)
		for (std::shared_ptr<Ent>& target : entsSelected)
		{
			Trace_t trace;
			Trace_t secondTraceHead;
			Trace_t thirdTraceLegs;
			int clipmask = 0x1011;

			const int kMaxFps = 60;
			const int kHeadLevelAdjustment = 3;
			const int kHeadLevelInBlockAdjustment = 4;
			const float kRollingDekaAdjustment = 1.5f;

			TraceLine(&trace, meAim->pos, 0, 0, target->predPos, meAim->id, clipmask, 0);
			if (slowFps > kMaxFps)
			{
				if ((trace.entityNum == target->id
					|| (trace.entityNum == target->dekaNum && target->entClass == CL_DEKA)
					|| trace.entityNum == 1023)
					&& (target->stance != 928 && target->stance != 929 && target->stance 
						!= 930 && target->stance != 669))
				{
					target->isVisible = true;
				}
				else
				{
					//aim at head level
					if (target->stance != 930 && target->stance != 669)
					{
						target->posPing[2] += target->realModelHeight / kHeadLevelAdjustment;
						target->predPos[2] += target->realModelHeight / kHeadLevelAdjustment;
					}
					else if (target->stance == 930 || target->stance == 669)
					{
						target->posPing[2] += target->realModelHeight / kHeadLevelInBlockAdjustment;
						target->predPos[2] += target->realModelHeight / kHeadLevelInBlockAdjustment;
					}

					TraceLine(&secondTraceHead, meAim->pos, 0, 0, target->predPos, meAim->id, clipmask, 0);

					if (secondTraceHead.entityNum == target->id
						|| (trace.entityNum == target->dekaNum && target->entClass == CL_DEKA)
						|| secondTraceHead.entityNum == 1023)
					{
						target->isVisible = true;
					}
					else
					{
						if ((secondTraceHead.entityNum != target->id
							|| trace.entityNum == target->dekaNum
							&& secondTraceHead.entityNum != 1023))
						{
							target->predPos[2] -= target->realModelHeight / kRollingDekaAdjustment;
							target->posPing[2] -= target->realModelHeight / kRollingDekaAdjustment;
							TraceLine(&thirdTraceLegs, meAim->pos, 0, 0, 
								target->predPos, meAim->id, clipmask, 0);
							if (thirdTraceLegs.entityNum == target->id
								|| trace.entityNum == target->dekaNum
								|| thirdTraceLegs.entityNum == 1023)
							{
								target->isVisible = true;
							}
						}
					}



				}
			}
			else
			{
				if ((trace.entityNum == target->id
					|| (trace.entityNum == target->dekaNum && target->entClass == CL_DEKA)
					|| trace.entityNum == 1023)
					)
				{
					target->isVisible = true;
				}
			}


		}
	else
	{
		for (std::shared_ptr<Ent>& target : entsSelected)
		{
			target->isVisible = true;
		}
	}
	//std::cout << std::endl;
}

//-------------------------------------------------------------//
std::shared_ptr<Ent> Aim::FindClosestToCrosshair(std::vector<std::shared_ptr<Ent>>& entsSelected)
{
	
	if (entsSelected.size())
	{
		std::shared_ptr<Ent> closestEntToCrosshairZero = entsSelected[0];
		for (std::shared_ptr<Ent>& player : entsSelected)
		{
			if ( player->anglesDistances.angleTo <= closestEntToCrosshairZero->anglesDistances.angleTo)
			{
				closestEntToCrosshairZero = player;
			}
		}
		return closestEntToCrosshairZero;
	}
	else
	{
		std::shared_ptr<Ent> noPlayer(new Ent());
		return noPlayer;
	}
}

//-------------------------------------------------------------//
void Aim::Pblocks(std::shared_ptr<Ent> &enemy)
{
	Weapons weapons;
	const float pbPitch = 25;
	const float pbYaw = 35;
	bool swordDetector = false;
	std::string swing;
	pblocksAngles pblocksAngles;
	pblocksAngles.swordPitch = 0.0;
	pblocksAngles.swordYaw = 0.0;


	if (enemy->anglesDistances.distanceAbs < 110 && meAim->weapon == Weapons::WP_LS)
	{
		if (enemy->saberAnim == 73 || enemy->saberAnim == 9 || enemy->saberAnim == 89) //AW     //129 BODYHIT
		{
			swordDetector = true;
			if (abs(pblocksAngles.swordPitch) < 50)
			{
				pblocksAngles.swordPitch = -pbPitch;
				pblocksAngles.swordYaw = pbYaw;
				swing = "AW";
			}
		} //AW

		if (enemy->saberAnim == 69 || enemy->saberAnim == 5 || enemy->saberAnim == 110) //D
		{
			swordDetector = true;
			if (abs(pblocksAngles.swordPitch) < 50)
			{
				pblocksAngles.swordPitch = 1;
			
				pblocksAngles.swordYaw = -pbYaw;
				swing = "D";
			}
		} //D

		if (enemy->saberAnim == 72 || enemy->saberAnim == 8 || enemy->saberAnim == 82) //A
		{
			swordDetector = true;
			if (abs(pblocksAngles.swordPitch) < 50)
			{
				pblocksAngles.swordPitch = 1;
				pblocksAngles.swordYaw = pbYaw;
				swing = "A";
			}
		} //A

		if (enemy->saberAnim == 68 || enemy->saberAnim == 4 || enemy->saberAnim == 116) //WD
		{
			swordDetector = true;
			if (abs(pblocksAngles.swordPitch) < 50)
			{
				pblocksAngles.swordPitch = -pbPitch;
				pblocksAngles.swordYaw = -pbYaw;
				swing = "WD";
			}
		} //WD

		if (enemy->saberAnim == 74 || enemy->saberAnim == 10 || enemy->saberAnim == 90) //W
		{
			swordDetector = true;
			if (abs(pblocksAngles.swordPitch) < 50)
			{
				pblocksAngles.swordPitch = -pbPitch;
				pblocksAngles.swordYaw = -1;
				swing = "W";
			}
		} //W

		if (enemy->saberAnim == 71 || enemy->saberAnim == 7 || enemy->saberAnim == 88) //AS
		{
			swordDetector = true;
			if (abs(pblocksAngles.swordPitch) < 50)
			{
				pblocksAngles.swordPitch = pbPitch;
				pblocksAngles.swordYaw = pbYaw;
				swing = "AS";
			}
		} //AS

		if (enemy->saberAnim == 70 || enemy->saberAnim == 6 || enemy->saberAnim == 117) //SD
		{
			swordDetector = true;
			if (abs(pblocksAngles.swordPitch) < 50)
			{
				pblocksAngles.swordPitch = pbPitch;
				pblocksAngles.swordYaw = -pbYaw;
				swing = "SD";
			}
		} //SD
	} //Pblocks

	//std::cout << swing << std::endl;
	if (abs(pblocksAngles.swordPitch) < 50)
	{

		if (swordDetector == false)
		{
			pblocksAngles.swordPitch = 0;
			pblocksAngles.swordYaw = 0;
			swing = "--";
		}

	}  //null
	//return (swing);


	enemy->anglesDistances.angleSetHor += pblocksAngles.swordYaw;
	enemy->anglesDistances.angleSetVert += pblocksAngles.swordPitch;
}

//-------------------------------------------------------------//
std::shared_ptr<Ent> Aim::ChooseTarget(std::shared_ptr<Ent> &chosenTarget, const bool &isTk, bool &isAim)
{
	std::vector<std::shared_ptr<Ent>> bufferEnts;
	std::vector<std::shared_ptr<Ent>> bufferEntsVis;
	std::vector<std::shared_ptr<Ent>> targetEntsVis;
    std::shared_ptr<Ent> retEnt = nullptr;
	std::shared_ptr<Ent> closestPlayer = nullptr;
	std::shared_ptr<Ent> closestGunner = nullptr;
	std::shared_ptr<Ent> lowestHpPlayer = nullptr;
	std::shared_ptr<Ent> chosenUpdated = nullptr;
	std::shared_ptr<Ent> meleeWook = nullptr;
	std::shared_ptr<Ent> gunAttackBySaber = nullptr;
	std::shared_ptr<Ent> saberAttackBySaber = nullptr;
	std::shared_ptr<Ent> target = nullptr;
	int lowestHp = 400;
	bool noTargets = true;

	const float kWookSafeDistance = 300;
	const float kJediSafeDistance = 110;

    if (!entsAliveAim.size())
        return retEnt;
	if (isTk)
	{
		bufferEnts = entsAliveAim;
	}
	else
		bufferEnts = entsEnemiesAim;

	//target select
	if (!varsMap[VAR_AIMTARGET])
	{
		for (std::shared_ptr<Ent>& player : entsAliveAim)
		{
			if (player->isTarget && player->isVisible)
			{
				targetEntsVis.push_back(player);
			}

		}
		for (std::shared_ptr<Ent>& player : entsAliveAim)
		{
			if (player->isTarget)
			{
				noTargets = false;
			}
		}
		if (targetEntsVis.size())
		{
			target = targetEntsVis[0];
			for (std::shared_ptr<Ent>& player : targetEntsVis)
			{
				if (player->anglesDistances.angleTo <= target->anglesDistances.angleTo)
					target = player;
			}
		}
		if (target)
		{
			return target;
		}
		if (!target && !noTargets && varsMap[VAR_TARGETEXCLUSIVE])
		{
			return nullptr;
		}
	}

	if (!bufferEnts.size())
		return retEnt;
	//target select

	for (std::shared_ptr<Ent>& player : bufferEnts)
	{
		if (player->isVisible && !player->isFriend)
			bufferEntsVis.push_back(player);
	}

	if (!bufferEntsVis.size())
		return retEnt;

	closestPlayer = bufferEntsVis[0];
	closestGunner = bufferEntsVis[0];
	for (std::shared_ptr<Ent>& player : bufferEntsVis)
	{
		//closest player
		if (player->anglesDistances.angleTo <= closestPlayer->anglesDistances.angleTo)
		{
			closestPlayer = player;
		}

		//closest gunner
		if (
			player->weapon != WP_LS
			|| (player->weapon == WP_LS && player->swordOn == 2)
			|| (player->weapon == WP_LS && player->swordOn == 0 && player->anglesDistances.angleFrom > 90)
			|| (player->weapon == WP_LS && (player->stance2 == 1354 || player->stance2 == 1358))
			|| (player->weapon == WP_LS && player->swordOn == 0) && player->stance2 == 928
			)
		{
			if (player->anglesDistances.angleTo <= closestGunner->anglesDistances.angleTo)
			{
				closestGunner = player;
			}
		}

		//lowest hp
		if (
			(player->hp != 100 && player->weapon != WP_LS)
			|| (player->hp != 100 && player->weapon == WP_LS && player->swordOn == 2)
			|| (player->weapon == WP_LS && player->swordOn == 0 && player->anglesDistances.angleFrom > 90)
			|| (player->weapon == WP_LS && (player->stance2 == 1354 || player->stance2 == 1358))
			|| (player->weapon == WP_LS && player->swordOn == 0) && player->stance2 == 928
			)
			if (player->hp < lowestHp)
			{
				lowestHp = player->hp;
				lowestHpPlayer = player;
			}

		if (chosenTarget && chosenTarget->id == player->id)
			chosenUpdated = player;

		//melee wook
		if (player->entClass == CL_WOOK && player->weapon == WP_MELEE 
			&& player->anglesDistances.distanceAbs < kWookSafeDistance)
		{
			meleeWook = player;
		}

		//if i have a gun and get attacked by a lightsaber
		if (meAim->weapon != Weapons::WP_MELEE && meAim->weapon != Weapons::WP_LS 
			&& player->anglesDistances.distanceAbs < kJediSafeDistance && player->saberAnim>1 
			&& player->saberAnim < 135 /* && varsMap["autoTargetjedi"]*/)
		{
			gunAttackBySaber = player;
			break;
		}

		//aim at attacking saberist and pblock
		if (meAim->weapon == Weapons::WP_LS && player->anglesDistances.distanceAbs < kJediSafeDistance 
			&& player->saberAnim>1 && player->saberAnim < 120)
		{

			if (varsMap[VAR_AUTOPB])
				Pblocks(player);
			if (varsMap[VAR_AUTOTARGETJEDI])
			{
				isAim = true;
			}
			saberAttackBySaber = player;
			break;

		}
	}

	if (varsMap[VAR_SWITCHONKILL] == 0)
	{
		if (chosenUpdated)
			return chosenUpdated;
		else
			return nullptr;
	}


    if (closestPlayer)
        retEnt = closestPlayer;
    if (closestGunner)
        retEnt = closestGunner;
    if (lowestHpPlayer)
        retEnt = lowestHpPlayer;
    if (meleeWook)
        retEnt = meleeWook;
	if (chosenUpdated && chosenUpdated->isVisible)
		retEnt = chosenUpdated;
    if (gunAttackBySaber)
        retEnt = gunAttackBySaber;
    if (saberAttackBySaber)
        retEnt = saberAttackBySaber;

    return retEnt;
}

//-------------------------------------------------------------//
float Aim::ExtremeAngleAdjustment(float angleSetY) const
{
	//entClasses Classes;

	float adjustedAngle = angleSetY;
	if (meAim->entClass == Classes::CL_DEKA)
	{
		if (angleSetY > 45 && angleSetY < 90)
			adjustedAngle = 45;
		else if (angleSetY < 330 && angleSetY > 270)
			adjustedAngle = 330;
	}
	else
	{
		if (angleSetY > 87 && angleSetY < 90)
			adjustedAngle = 87;
		else if (angleSetY < 280 && angleSetY > 270)
			adjustedAngle = 280;
	}
	return adjustedAngle;
}

//-------------------------------------------------------------//
float Aim::Angle360EnemyToMe(std::shared_ptr<Ent> &enemy)
{
	float enMyYaw360 = 0;
	float enYawZero = 0;
	float angleSetEnemy;
	vec3_t enemyDeltas;
	VecCopy(enemyDeltas, enemy->pos);
	VecSub(enemyDeltas, meAim->pos);
	enYawZero -= enemy->enYaw + 90;

	angleSetEnemy = (AngleBetweenEnemyX(enemyDeltas[1], enemyDeltas[0]));

	enMyYaw360 = enYawZero - angleSetEnemy;
	if (enMyYaw360 < -360)
		enMyYaw360 += 360;
	if (enMyYaw360 > 360)
		enMyYaw360 -= 360;

	if (enMyYaw360 < -180)
		enMyYaw360 += 360;

	return enMyYaw360;
}

//-------------------------------------------------------------//
void Aim::CalcAngleToSet(std::vector<std::shared_ptr<Ent>>& players, const bool isPrediction)
{
	if (players.size())
	{
		for (std::shared_ptr<Ent>& enemy : players)
		{
			float angle360EnemytoMe = 0;
			float adjustLeaningEnemy = 0;
			float enYawZero = 0;
			float enPitchZero = 0;
			float angleSet;
			float angleSetY;
			float dekaAdj = 1;
			vec3_t deltas;



			VecCopy(deltas, meAim->pos);
			if (isPrediction)
			{
				enemy->anglesDistances.distanceXY = VecDistance2(meAim->pos, enemy->predPos);
				enemy->anglesDistances.distanceAbs = VecDistance(meAim->pos, enemy->predPos);
				VecSub(deltas, enemy->predPos);
			}
			else
			{
				enemy->anglesDistances.distanceXY = VecDistance2(meAim->pos, enemy->posPing);
				enemy->anglesDistances.distanceAbs = VecDistance(meAim->pos, enemy->posPing);
				VecCopy(enemy->predPos, enemy->posPing);
				VecSub(deltas, enemy->posPing);
			}
				

			angle360EnemytoMe = Angle360EnemyToMe(enemy);
			adjustLeaningEnemy = (float)(6 * sin(angle360EnemytoMe * kDegreesToRad) * sin(enemy->enPitch * kDegreesToRad) * 100.0f / enemy->anglesDistances.distanceAbs);

			if (enemy->stance == 1204)
				adjustLeaningEnemy = adjustLeaningEnemy / 2;

			//CastVerticalRayDownFromEnemy(enemy);
			enYawZero -= meAim->yaw + 90;
			enPitchZero -= meAim->pitch;

			angleSetY = (AngleBetweenEnemyX(enemy->anglesDistances.distanceXY, -deltas[2]));
			angleSet = (AngleBetweenEnemyX(deltas[1], deltas[0])) - enemy->weaponInfo->shoulderAdj - adjustLeaningEnemy; // 1 * adjustLeaningEnemy;

			angleSet = enYawZero - angleSet;
			angleSetY = enPitchZero - angleSetY;
			if (angleSet < -360)
				angleSet += 360;
			else if (angleSet > 360)
				angleSet -= 360;

			if (angleSet < -180)
				angleSet += 360;

			if (angleSetY < -360)
				angleSetY += 360;
			else if (angleSetY > 360)
				angleSetY -= 360;

			if (angleSetY < -180)
				angleSetY += 360;

			if (isnan(angleSet) || isnan(angleSetY))
			{
				angleSet = 0;
				angleSetY = 0;
			}
			enemy->anglesDistances.angleSetHor = angleSet;
			enemy->anglesDistances.angleSetVert = ExtremeAngleAdjustment(angleSetY);
		}
	}
}

//-------------------------------------------------------------//
bool Aim::AimLock(std::shared_ptr<Ent> &enemy, int &iterations)
{
	const float kMinAngle = 12.0f;

	if (!enemy)
		return 0;
	if (iterations == 0)
		iterations = 1;
	memCoordX.clear();
	memCoordY.clear();

	float baseAngleX = enemy->anglesDistances.angleSetHor / iterations;
	float baseAngleY = enemy->anglesDistances.angleSetVert / iterations;

	float newAngleX = 0;
	float newAngleY = 0;

	for (int i = 0; i < iterations; i++)
	{
		newAngleX += baseAngleX;
		newAngleY += baseAngleY;
		if (enemy->anglesDistances.angleTo > kMinAngle)
		{
			memCoordX.push_back(meAim->myRawYaw + newAngleX);
			memCoordY.push_back(meAim->myRawPitch + newAngleY);
		}
	}
	
	if (iterator < memCoordX.size())
	{
		*(float*)meAim->myRawYawAdd = memCoordX[iterator];
		*(float*)meAim->myRawPitchAdd = memCoordY[iterator];
	}
	if (iterator >= memCoordX.size())
	{
		iterator = 0;
	}
	else
	{
		iterator += 1;
	}

	if (enemy->anglesDistances.angleTo < kMinAngle)
	{
		*(float*)meAim->myRawYawAdd = meAim->myRawYaw + enemy->anglesDistances.angleSetHor;
		*(float*)meAim->myRawPitchAdd = meAim->myRawPitch + enemy->anglesDistances.angleSetVert;
	}
}

//-------------------------------------------------------------//
void Aim::GetScreenCoordinates(std::vector<std::shared_ptr<Ent>>& entsSelected, const bool predictDraw)
{
	for (std::shared_ptr<Ent>& player : entsSelected)
	{
		vec2_t screenCrd;
		vec2_t screenFeet;
		vec2_t screenHead;
		vec3_t bufferCrd;

		if (predictDraw)
		{
			VecCopy(bufferCrd, player->predPos);
		}
		else
		{
			VecCopy(bufferCrd, player->pos);
			VecCopy(player->feetPos, bufferCrd);
			VecCopy(player->headPos, bufferCrd);
			player->feetPos[2] -= (player->realModelHeight / 3 + player->realModelHeight / 2.6);
			player->headPos[2] += player->realModelHeight / 6;
		}

		worldToScreen.WTS(player->predPos, player->info2d.screenCrd);
		worldToScreen.WTS(player->feetPos, player->info2d.screenFeet);
		worldToScreen.WTS(player->headPos, player->info2d.screenHead);
		worldToScreen.WTS(meAim->pos, player->info2d.screenMyPos);
		player->info2d.modelHeight = player->info2d.screenHead[1] - player->info2d.screenFeet[1];
		player->info2d.modelWidth = player->info2d.modelHeight / 2;
	}
}

//-------------------------------------------------------------//
void Aim::EntCalculations(const int& slowFps, Actions& actions,
	const bool& targetThroughWalls,const bool& isPrediction)
{
	ModelHeightAdjustment(meAim, entsAliveAim);
	CalcDistAngles(entsAliveAim);
	Adjustments(entsAliveAim);
	isVisible(entsAliveAim, slowFps, targetThroughWalls);
	CrouchWhenAttacked(actions);
	CalcAngleToSet(entsAliveAim, isPrediction);
}