#include "hook.h"


hook::Hook::Hook(BYTE* src, BYTE* dst, BYTE* gatewayPtr, uintptr_t len)
{
	this->src = src;
	this->dst = dst;
	this->gatewayPtr = gatewayPtr;
	this->len = len;
}
hook::Hook::Hook(const char* exportName, const char* modName, BYTE* dst, BYTE* gatewayPtr, uintptr_t len)
{
	HMODULE hMod = GetModuleHandleA(modName);
	if (hMod == 0)
	{
		return;
	}

	this->src = (BYTE*)GetProcAddress(hMod, exportName);
	this->dst = dst;
	this->gatewayPtr = gatewayPtr;
	this->len = len;
}

void hook::Hook::Enable()
{

}
void hook::Hook::Disable()
{

}
void hook::Hook::Toggle()
{

}



/// <summary>
/// Overwrites bytes at source address with a jump to destination address
/// </summary>
/// <param name="src">Address to write jump at</param>
/// <param name="dst">Destination to jump to</param>
/// <param name="len">lenth of jump (5)</param>
/// <returns>True</returns>
bool hook::Detour32(BYTE* src, BYTE* dst, const uintptr_t len)
{
	// we need at least 5 bytes for the jump
	if (len < 5) return false;

	DWORD oldProtect;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &oldProtect);

	uintptr_t relativeAddress = (uintptr_t)(dst - src) - 5;

	*src = 0xE9; // JMP opcode

	*(uintptr_t*)(src + 1) = relativeAddress;

	VirtualProtect(src, len, oldProtect, &oldProtect);

	return true;
}

/// <summary>
/// Creates a trampoline hook
/// </summary>
/// <param name="src">Will be the address of the original opengl function</param>
/// <param name="dst">Will be the address of our new opengl hook function</param>
/// <param name="len"></param>
/// <returns></returns>
BYTE* hook::TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len)
{
	if (len < 5) return nullptr;

	// create gateway
	BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	// write stolen bytes to gateway
	memcpy_s(gateway, len, src, len);

	//Get the gateway to dest address
	uintptr_t  gatewayRelativeAddr = (uintptr_t)(src - gateway) - 5;

	//add the jump opcode to end of the gateway
	*(gateway + len) = 0xE9;

	// write the address of the gateway to the jump
	*(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelativeAddr;

	//perform the detour
	Detour32(src, dst, len);

	return gateway;
}
