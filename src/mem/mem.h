#pragma once

#include <windows.h>
#include <tlhelp32.h>

namespace mem
{

	void PatchExternal(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);

	void NopExternal(BYTE* dst, unsigned int size, HANDLE hProcess);



}