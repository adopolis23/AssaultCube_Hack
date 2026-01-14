#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>


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



int main(int argc, char** argv)
{
	const wchar_t* programName = L"ac_client.exe";
	const wchar_t* dllPath = L"E:/dev/AssaultCube_Hack/bin/Debug/AssaultCube_Hack_DLL.dll";


	DWORD pid = GetProcessIdByName(programName);
	uintptr_t moduleBase = GetModuleBaseAddress(pid, programName);


	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL)
	{
		printf("Failed to get handle to process.\n");
		return 1;
	}

	// allocate string in remote process
	LPVOID remoteString = VirtualAllocEx(hProcess, NULL, (wcslen(dllPath) + 1) * sizeof(wchar_t), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (remoteString == NULL)
	{
		printf("Failed to allocate remote string.\n");
		return 1;
	}

	if (!WriteProcessMemory(hProcess, remoteString, dllPath, (wcslen(dllPath) + 1) * sizeof(wchar_t), NULL))
	{
		printf("Failed to write remote string.\n");
		VirtualFreeEx(hProcess, remoteString, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return 1;
	}

	HMODULE hKernel32 = GetModuleHandleA("Kernel32");
	if (!hKernel32)
	{
		printf("Failed to get handle to kernel32.\n");
		VirtualFreeEx(hProcess, remoteString, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return 1;
	}

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW"), remoteString, 0, NULL);
	if (!hThread)
	{
		printf("Failed to create LoadLibraryW thread.\n");
		VirtualFreeEx(hProcess, remoteString, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return 1;
	}

	WaitForSingleObject(hThread, INFINITE);


	DWORD exitCode = 0;
	GetExitCodeThread(hThread, &exitCode);

	if (exitCode == 0)
	{
		printf("LoadLibraryW failed (DLL NOT loaded)\n");
	}
	else
	{
		printf("DLL loaded at remote base: 0x%p\n", (void*)exitCode);
	}












	VirtualFreeEx(hProcess, remoteString, 0, MEM_RELEASE);
	CloseHandle(hThread);
	CloseHandle(hProcess);

	printf("DLL Injected Successfully!");

	getchar();

	return 0;
}