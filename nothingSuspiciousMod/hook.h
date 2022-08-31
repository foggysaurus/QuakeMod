#pragma once

#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <WinDef.h>
#include <psapi.h>

namespace Hook
{

	// Detour32 - replace bytes with a jmp command
	bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len);

	// TrampHook32 - jump to the address specified then return to original point
	BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len);

	// ReturnBytes - restore everything as it was before (change jmp to original stolen bytes for
	//seamless detachment)
	bool ReturnBytes(BYTE* src, BYTE* dst, const uintptr_t len, BYTE* gateway);
}
