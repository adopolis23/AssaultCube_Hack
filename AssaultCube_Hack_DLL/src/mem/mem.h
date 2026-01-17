#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <vector>

namespace mem
{

	// Patches memory in an external process
	void PatchExternal(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);
	void NopExternal(BYTE* dst, unsigned int size, HANDLE hProcess);
	uintptr_t FindDMAAddress(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

	// Patches memory in internal process
	void PatchInternal(BYTE* dst, BYTE* src, unsigned int size);
	void NopInternal (BYTE* dst, unsigned int size);
	uintptr_t FindDMAAddressInternal(uintptr_t ptr, std::vector<unsigned int> offsets);

}