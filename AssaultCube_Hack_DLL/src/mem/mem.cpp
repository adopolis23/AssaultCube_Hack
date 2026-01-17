#include "mem.h"


void mem::PatchExternal(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
	DWORD oldProtections;
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldProtections);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, oldProtections, &oldProtections);
}

void mem::NopExternal(BYTE* dst, unsigned int size, HANDLE hProcess)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);

	PatchExternal(dst, nopArray, size, hProcess);

	delete[] nopArray;
}


uintptr_t mem::FindDMAAddress(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{

	uintptr_t address = ptr;
	for (unsigned int offset : offsets)
	{
		if (!ReadProcessMemory(hProc, (LPCVOID)address, &address, sizeof(address), nullptr))
		{
			return 0;
		}
		address += offset;
	}

	return address;
}



void mem::PatchInternal(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD oldProtections;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtections);

	memcpy(dst, src, size);

	VirtualProtect(dst, size, oldProtections, &oldProtections);
}

void mem::NopInternal(BYTE* dst, unsigned int size)
{
	DWORD oldProtections;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtections);

	memset(dst, 0x90, size);

	VirtualProtect(dst, size, oldProtections, &oldProtections);
}


uintptr_t mem::FindDMAAddressInternal(uintptr_t ptr, std::vector<unsigned int> offsets)
{

	uintptr_t address = ptr;
	for (unsigned int offset : offsets)
	{
		address = *(uintptr_t*)address;
		address += offset;
	}

	return address;
}


/// <summary>
/// Overwrites bytes at source address with a jump to destination address
/// </summary>
/// <param name="src">Address to write jump at</param>
/// <param name="dst">Destination to jump to</param>
/// <param name="len">lenth of jump (5)</param>
/// <returns>True</returns>
bool mem::Detour32(BYTE* src, BYTE* dst, const uintptr_t len)
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
BYTE* mem::TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len)
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