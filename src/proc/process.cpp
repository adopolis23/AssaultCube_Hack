#include "process.h"


void PrintProcessList()
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32W procEntry;
		procEntry.dwSize = sizeof(PROCESSENTRY32W);

		if (Process32FirstW(hSnap, &procEntry))
		{
			do
			{
				DWORD id = GetProcessIdByName(procEntry.szExeFile);
				
				_tprintf(TEXT("%s - %d\n"), procEntry.szExeFile, id);

			} while (Process32NextW(hSnap, &procEntry));
		}
		CloseHandle(hSnap);
	}

	printf("\n");
}

DWORD GetProcessIdByName(const wchar_t* processName)
{
	DWORD processId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32W procEntry;
		procEntry.dwSize = sizeof(PROCESSENTRY32W);

		if (Process32FirstW(hSnap, &procEntry))
		{
			do
			{
				if (wcscmp(procEntry.szExeFile, processName) == 0)
				{
					processId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32NextW(hSnap, &procEntry));
		}
		CloseHandle(hSnap);
	}

	return processId;
}


uintptr_t GetModuleBaseAddress(DWORD processId, const wchar_t* moduleName)
{
	uintptr_t moduleBaseAddress = 0;
	handle_t hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32W modEntry;
		modEntry.dwSize = sizeof(MODULEENTRY32W);
		if (Module32FirstW(hSnap, &modEntry))
		{
			do
			{
				if (wcscmp(modEntry.szModule, moduleName) == 0)
				{
					moduleBaseAddress = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32NextW(hSnap, &modEntry));
		}
		CloseHandle(hSnap);
	}

	return moduleBaseAddress;
}

uintptr_t FindDMAAddress(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
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