#include "pch.h"
#include "actions.h"

//-------------------------------------------------------------//
void Actions::SetActions(const AddressesList& addresses)
{
    movForward.action = addresses.w;
    movBack.action = addresses.s;
    movLeft.action = addresses.a;
    movRight.action = addresses.d;
    crouch.action = addresses.crouch;
    att.action = addresses.att;
    altAtt.action = addresses.altAttack;
    jump.action = addresses.jump;
}

//-------------------------------------------------------------//
void Actions::WriteToMem(uintptr_t address, int value)
{
    if (address)
        *(int*)address = value;
}

//-------------------------------------------------------------//
bool Actions::Do(ActionFlag &action)
{
    if (!action.action)
        return false;
    if (!action.actionFlag)
    {
        WriteToMem(action.action, 1);
        action.actionFlag = true;
    }
}

//-------------------------------------------------------------//
bool Actions::Stop(ActionFlag& action)
{
    if (!action.action)
        return false;
    if (action.actionFlag)
    {
        WriteToMem(action.action, 0);
        action.actionFlag = false;
    }
}

//-------------------------------------------------------------//
bool Actions::AmIDoing(ActionFlag &action)
{
    if (!action.action)
        return false;
    // return (*(bool*)action.action);
    if (*(bool*)action.action)
        return true;
    else
        return false;
}
