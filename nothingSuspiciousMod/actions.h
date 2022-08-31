#pragma once
#include "memory.h"
#include <iostream> 

///////////////////////////////////////////////////////////////
// class Actions actions performed by bot account
class Actions
{

private:
	bool flagForward;
	struct ActionFlag
	{
		uintptr_t action;
		bool actionFlag;
	};

public:
	//SetActions copy addressess values to struct
	void SetActions(const AddressesList& addresses);

	//WriteToMem write value to game memory
	void WriteToMem(uintptr_t address, int value);

	//Do perform action
	bool Do(ActionFlag &action);

	//Stop performing action
	bool Stop(ActionFlag& action);
	///////////////////////////////////////////////////////////////
	//AmIDoing return positive if performing action
	bool AmIDoing(ActionFlag &action);

public:
	ActionFlag movForward;
	ActionFlag movBack;
	ActionFlag movLeft;
	ActionFlag movRight;
	ActionFlag crouch;
	ActionFlag att;
	ActionFlag altAtt;
	ActionFlag jump;
};