#include "pch.h"
#include "ent.h"

void Ent::FillDeka(uintptr_t dekaBase)
{
    dekaNum = *(int*)(dekaBase - 4);
    id = *(int*)(dekaBase + 0x114);
    pos[0] = *(float*)(dekaBase + 0x18);
    pos[1] = *(float*)(dekaBase + 0x18 + 0x4);
    pos[2] = *(float*)(dekaBase + 0x18 + 0x8);
    speed[0] = *(float*)(dekaBase + 0x24);
    speed[1] = *(float*)(dekaBase + 0x24 + 0x4);
    speed[2] = *(float*)(dekaBase + 0x24 + 0x8);
    stance = *(int*)(dekaBase + 0x128);
    stance2 = *(int*)(dekaBase + 0x12C);
    deadOrAlive = *(int*)(dekaBase + 0x444);
    enPitch = *(float*)(dekaBase + 0x74);
    enYaw = *(float*)(dekaBase + 0x78);
    isRendered = 1;
    entClass = 11;
    weapon = 4;
}

//-------------------------------------------------------------//
void Ent::FillEnt(uintptr_t mbiiBase, uintptr_t entBase)
{
    id = *(int*)(entBase);
    speed[0] = *(float*)(entBase + 0x28);
    speed[1] = *(float*)(entBase + 0x2C);
    speed[2] = *(float*)(entBase + 0x30);
    hp = *(int*)(entBase + 0x170);
    isDisrupted = *(bool*)(entBase + 0x644);
    enPitch = *(float*)(entBase + 0x40);
    enYaw = *(float*)(entBase + 0x44);
    surface = *(int*)(entBase + 0xC4);
    saberAnim = *(int*)(entBase + 0xF8);
    stance = *(int*)(entBase + 0x12C);
    stance2 = *(int*)(entBase + 0x130);
    swordOn = *(int*)(entBase + 0x100);
    pos[0] = *(float*)(entBase + 0x5AC);
    pos[1] = *(float*)(entBase + 0x5B0);
    pos[2] = *(float*)(entBase + 0x5B4);
    charScale = *(int*)(entBase + 0x14C);
    isRendered = *(int*)(entBase + 0x448);
    deadOrAlive = *(int*)(entBase + 0x5f8); //5f8 //5FC minigun doesnt work
    weapon = *(int*)(entBase + 0x128);
    numWeapon = *(int*)(entBase + 0x128);
}

//-------------------------------------------------------------//
void Me::FillMe(AddressesList& address,  uintptr_t myTrueId, uintptr_t playerOffset)
{
    trueId = myTrueId;
    myCharScale = *(int*)(address.entBase + 0x14C + myTrueId * playerOffset);
    myRawPitch = *(float*)(address.myRawPitch);
    myRawPitchAdd = (int*)(address.myRawPitch);
    myRawYaw = *(float*)(address.myRawYaw);
    myRawYawAdd = (int*)(address.myRawYaw);
    myShield = *(int*)(address.myShield);
    ping = *(int*)(address.ping);
    WristBlaster = *(int*)(address.myWristBlaster);
    IsAltFire = *(int*)(address.IsAltFire);
    IsScope = *(int*)(address.IsScope); //4 scope
    isOnFire = *(int*)(address.isOnFire); //2 on fire
    moveDown = *(int*)(address.moveDown);
    ammo = *(int*)(address.ammo);
    amAttacking = *(bool*)(address.att);
    amAltAttacking = *(bool*)(address.altAttack);
    cameraPos[0] = *(float*)(address.cameraPos);
    cameraPos[1] = *(float*)(address.cameraPos + 4);
    cameraPos[2] = *(float*)(address.cameraPos + 8);

    uintptr_t offsetTeam = trueId * address.teamOffset;
    uintptr_t offsetPlayer = trueId * address.playerOffset;

    totalAmmo = *(int*)(address.totalAmmo + offsetTeam);
    clipAmmo = *(int*)(address.totalAmmo + offsetTeam + 4);
    yaw = *(float*)(address.entBase - 0x4BFD880);
    pitch = *(float*)(address.entBase - 0x4BFD880 - 4);
    pitch360 = *(float*)(address.entBase + 0x234DA8);
    yaw360= *(float*)(address.entBase + 0x234DA8 + 4);
    amIdead = *(int*)(address.entBase + 0x2598D0);
    tked = *(int*)(address.entBase - 0x4E366B8);
    myHp = *(int*)(address.entBase + 0x234194);
    lastAttackerNum = *(int*)(address.entBase + 0x2341EC);
    entInCrosshair = *(int*)(address.entBase + 0x23754C);
    tkPoints = *(int*)(address.entBase + 0x2341C8);
    whereImLooking[0] = *(float*)(address.entBase - 0x4E3665C);
    whereImLooking[1] = *(float*)(address.entBase - 0x4E3665C + 0x4);
    whereImLooking[2] = *(float*)(address.entBase - 0x4E3665C + 0x8);

    lookingToUVec[0] = *(float*)(address.entBase - 0x2A8AD0);
    lookingToUVec[1] = *(float*)(address.entBase - 0x2A8AD0 + 0x4);
    lookingToUVec[2] = *(float*)(address.entBase - 0x2A8AD0 + 0x8);
    
}

//-------------------------------------------------------------//
bool WorldToScreen::WTS(vec3_t point, vec2_t& result)
{
    int	xcenter, ycenter;
    vec3_t	local, transformed;
    vec3_t	vfwd, vright, vup;
    vec3_t worldCoord;
    int minusX = 1;
    int minusY = 1;
    VecCopy(worldCoord, point);

    //NOTE: did it this way because most draw functions expect virtual 640x480 coords
    //	and adjust them for current resolution
    xcenter = res[0] / 2;//gives screen coords in virtual 640x480, to be adjusted when drawn
    ycenter = res[1] / 2;//gives screen coords in virtual 640x480, to be adjusted when drawn

    VectorSubtract(worldCoord, me->cameraPos, local);

    AngleVectors(me->yaw360, me->pitch360, 0, vfwd, vright, vup);

    transformed[0] = DotProduct(local, vright);
    transformed[1] = DotProduct(local, vup);
    transformed[2] = DotProduct(local, vfwd);

    // Make sure Z is not negative.

    if (transformed[2] < 12)
    {
        transformed[2] = -transformed[2];
        minusX = 0;
        minusY = 0;
    }

    float testX = fov[0]; //regular //75.0   * 1.5f
    float testY = (fov[1]); //75.0   * 1.27f
    float div = fov[0] / fov[1];

    //float adjustment = 1 / (-2 * 112.9/fov[0] * 5.2 + 7.4) + 1;
    float adjustment = 1 / (-3.03 * 112.9 / fov[0] * 5.1 + 12.5) + 1;
    float adjustmentVert = 1 / (-10 * 80.57 / fov[1] * 3.8 + 30.5) + 1;

    float xzi = xcenter / transformed[2] * (112.9 / fov[0] * adjustment);
    float yzi = ycenter / transformed[2] * (112.9 / fov[1] * adjustmentVert); //112 in scope //93 no

    result[0] = (xcenter + xzi * transformed[0]);
    result[1] = (ycenter - yzi * transformed[1]);
    result[0] *= minusX;
    result[1] *= minusY;

    return true;
}
