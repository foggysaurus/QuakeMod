
#include "pch.h"
#include "hook.h"



bool Hook::Detour32(BYTE* src, BYTE* dst, const uintptr_t len)
{
	if (len < 5) return false;

	DWORD  curProtection;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

	uintptr_t  relativeAddress = dst - src - 5;

	*src = 0xE9;
	*(uintptr_t*)(src + 1) = relativeAddress;

	VirtualProtect(src, len, curProtection, &curProtection);
	return true;
}

//-------------------------------------------------------------//
BYTE* Hook::TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len)
{
	// Make sure the length is greater than 5
	if (len < 5) return 0;

	// Create the gateway (len + 5 for the overwritten bytes + the jmp)
	BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	//Write the stolen bytes into the gateway
	if (gateway)
		memcpy(gateway, src, len);

	// Get the gateway to destination addy 
	uintptr_t  gatewayRelativeAddr = src - gateway - 5;

	// Add the jmp opcode to the end of the gateway
	*(gateway + len) = 0xE9;

	// Add the address to the jmp
	*(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelativeAddr;

	// Perform the detour
	Detour32(src, dst, len);

	return gateway;
}

//-------------------------------------------------------------//
bool Hook::ReturnBytes(BYTE* src, BYTE* dst, const uintptr_t len, BYTE* gateway)
{
	if (len < 5) return false;

	DWORD  curProtection;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

	uintptr_t  relativeAddress = dst - src - 5;

	memcpy(src, gateway, len);

	VirtualProtect(src, len, curProtection, &curProtection);
	return true;
}