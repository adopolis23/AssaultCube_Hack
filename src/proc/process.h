#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <tchar.h>

void PrintProcessList();

DWORD GetProcessIdByName(const wchar_t* processName);

uintptr_t GetModuleBaseAddress(DWORD processId, const wchar_t* moduleName);
