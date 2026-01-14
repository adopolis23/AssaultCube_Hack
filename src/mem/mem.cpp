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