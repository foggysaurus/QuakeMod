#pragma once
#pragma comment(lib, "OpenGL32.lib")

#include <string>
#include <vector>
#include <stdio.h>
#include <Windows.h>
#include <memory>
#include "vec.h"
#include "memory.h"
#include <gl\GL.h>

///////////////////////////////////////////////////////////////
// struct AnglesDistances - angles and distances to players from me 
struct AnglesDistances
{
    float distanceXY;
    float distanceAbs;
    float angleTo;
    float angleFrom;
    float angleSetHor;
    float angleSetVert;
};

///////////////////////////////////////////////////////////////
// struct WeaponInfo - adjustments for the weapon im holding
struct WeaponInfo
{
    float shoulderAdj;
    float distAdjust;
    float projvelAdj;
};

///////////////////////////////////////////////////////////////
// struct Info2d - 2d render of 3d enemy coordinates and relevant data
struct Info2d
{
    vec2_t screenCrd;
    vec2_t screenFeet;
    vec2_t screenHead;
    vec2_t screenMyPos;
    float modelWidth;
    float modelHeight;
};

///////////////////////////////////////////////////////////////
// struct ColorInfo - ESP colors and other settings
struct ColorInfo
{
     GLubyte colorEnemy[3];
     GLubyte colorEnemyInvis[3];
     GLubyte colorAlly[3];
     GLubyte colorTarget[3];
     GLubyte colorAllyInvis[3];
     GLubyte colorFriend[3];
     GLubyte colorFriendInvis[3];
     GLubyte colorTargetInvis[3];
     float linesEnemyThickness = 1;
     float linesTargetThickness = 1;
     float linesAllyThickness = 1;
     float linesFriendThickness = 1;
     float boxEnemyThickness = 1;
     float boxAllyThickness = 1;
     float boxFriendThickness = 1;
     float boxTargetThickness = 1;
     int fontSize = 9;
     bool bold = 0;
     bool italic = 0;
     std::string font = "Ariel";
};

///////////////////////////////////////////////////////////////
// struct ScreenData - screen FOV and resolution
struct ScreenData
{
    vec2_t fov;
    int res[2];
};

enum Weapons : int
{
    WP_NONE,
    WP_PROJ,
    WP_MELEE,
    WP_LS,
    WP_PISTOL,
    WP_E11,
    WP_DISR,
    WP_BOW,
    WP_CR,
    WP_CPISTOL,
    WP_DLT,
    WP_PLX,
    WP_T21 = 20,
    WP_SBD,
    WP_WPISTOL,

};

enum Classes : int
{
    CL_NONE,
    CL_SOLDIMP,
    CL_SOLD,
    CL_ETIMP,
    CL_ET,
    CL_SITH,
    CL_JEDI,
    CL_BH,
    CL_HERO,
    CL_SBD,
    CL_WOOK,
    CL_DEKA,
    CL_CLONE,
    CL_MANDA,
    CL_ARC,
};

enum permanentValuesList
{
    VAR_LOCK,
    VAR_SMOOTHAIM,
    VAR_WALLHACK,
    VAR_SWITCHONKILL,
    VAR_AUTOTARGETJEDI,
    VAR_ANTIKNOCKBACK,
    VAR_AIMTARGET,
    VAR_AUTOPB,
    VAR_ANTIAFK,
    VAR_TARGETEXCLUSIVE,
    VAR_DRAWBOXES,
    VAR_DRAWBOXES_ALLIES,
    VAR_DRAWBOXES_ENEMIES,
    VAR_DRAWBOXES_FRIENDS,
    VAR_DRAWBOXES_TARGETS,
    VAR_DRAWLINES,
    VAR_DRAWLINES_TOALLIES,
    VAR_DRAWLINES_TOENEMIES,
    VAR_DRAWLINES_FRIENDS,
    VAR_DRAWLINES_TARGETS,
    VAR_DRAW_ENEMY_STATS,
    VAR_DRAW_ALLY_STATS,
    VAR_DRAW_BIG_STATS,
    VAR_TOGGLETOHITSCAN,
    VAR_AUTOPUNISH,
};

///////////////////////////////////////////////////////////////
// struct PlayerNames - player name and truncated name
struct PlayerNames 
{
    int id;
    std::string name;
    std::string shortName;
    bool ignored;

public:
    PlayerNames():
        id(0), ignored(false)
    { }
};

///////////////////////////////////////////////////////////////
// class Ent - entity (player)
class Ent
{
public:
    int color1;
    int color2;
    int id;
    int surface;
    int saberAnim;
    int numWeapon;
    int stance;
    int stance2;
    int hp;
    int force;
    int charScale;
    int team;
    int myTeam;
    int isRendered;
    int entClass;
    int weapon;
    int dekaNum = -1;
    int swordOn;

    float distGround;
    float enYaw;
    float enPitch;
    float realModelHeight;

    bool isVisible;
    bool deadOrAlive;
    bool isClosestEnemy;
    bool isClosest;
    bool isDisrupted;

    bool isTarget;
    bool isFriend;
    bool isAlly;
    bool isEnemy;
    bool correctColor1;
    bool correctColor2;
    bool correctColor3;
    bool correctColor4;

    const char* className;
    std::string name;
    std::string shortName;

    Info2d info2d;

    vec3_t pos;
    vec3_t posPing;
    vec3_t feetPos;
    vec3_t headPos;
    vec3_t predPos;
    vec3_t speed;
    vec3_t groundContact;
    vec3_t shotEndpos;
   

    AnglesDistances anglesDistances;
    std::shared_ptr<WeaponInfo> weaponInfo;

public:
    
    // FillEnt - assign values to fields in my entity
    void FillEnt(uintptr_t mbiiBase, uintptr_t entBase);
    
// FillDeka - assign values to fields in droideka entities (are treated as NPCs by the game hence different
    //addresses
    void FillDeka(uintptr_t dekaBase);
public:
    virtual ~Ent() = default;
};


///////////////////////////////////////////////////////////////
// class me - my entity
class Me : public Ent
{
public:
    int myCharScale = 0;
    int* myRawYawAdd = nullptr;
    int* myRawPitchAdd = nullptr;
    int myHp;
    int myShield;
    int WristBlaster;
    int IsAltFire;
    int IsScope;
    int ping;
    int ammo;
    int totalAmmo;
    int clipAmmo;
    int lastAttackerNum;
    int entInCrosshair;
    int tkPoints;
    int tked;
    int trueId;
    vec3_t whereImLooking;
    vec3_t cameraPos;
    vec3_t lookingToUVec;

    float myRawYaw = 0;
    float myRawPitch = 0;

    float yaw;
    float pitch;
    float pitch360;
    float yaw360;
    float obstacleHeight;
    float depth;
    float depthBehind;
    float obstacleDistLeft;
    float obstacleDistRight;

    std::string forcepowers;
    std::string serverName;
    std::shared_ptr<Ent> lastAttacker;

    bool forward;
    bool back;
    bool left;
    bool right;
    bool enemiesAround;
    bool moveDown;
    bool isOnFire;
    bool amIdead;
    bool attack;
    bool altAttack;
    bool amIFollowing;
    bool amAttacking;
    bool amAltAttacking;
    

    void FillMe(AddressesList& address, uintptr_t myTrueId, uintptr_t playerOffset);
};

class WorldToScreen
{
public:
    vec2_t fov;
    int res[2];
    std::shared_ptr<Me> me;

public:
    bool WTS(vec3_t point, vec2_t& result);
};
///////////////////////////////////////////////////////////////
// struct UIInfo - struct to pass some values
struct UIInfo
{
    bool isTK;
    bool isThroughWalls;
    bool isHitscan;
    bool correctColor1;
    bool correctColor2;
    bool correctColor3;
    bool correctColor4;

    std::vector<std::shared_ptr<Ent>> targets;
    int posX;
    int posY;
};