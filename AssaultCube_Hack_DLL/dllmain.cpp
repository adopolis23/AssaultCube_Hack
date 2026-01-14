#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include "process.h"
#include "mem.h"

DWORD WINAPI HackThread(HMODULE hModule)
{
	//create console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);

	PrintProcessList();

	DWORD pid = GetProcessIdByName(L"ac_client.exe");

	uintptr_t moduleBase = GetModuleBaseAddress(pid, L"ac_client.exe");

	// get handle to process (requires admin)
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	// find the ammo address in this instance of the game
	uintptr_t dynamicPtrBaseAddrAmmoAr = moduleBase + 0x00183828;
	std::vector<unsigned int> ammoOffsets = { 0x8, 0x1C0, 0x324 };
	uintptr_t ammoAddr = mem::FindDMAAddressInternal(dynamicPtrBaseAddrAmmoAr, ammoOffsets);


	// find the health address in this instance of the game
	// this address is base address of the player object
	uintptr_t dynamicPtrBaseAddrHealth = moduleBase + 0x0017E0A8;
	std::vector<unsigned int> healthOffsets = { 0xEC };
	uintptr_t healthAddr = mem::FindDMAAddressInternal(dynamicPtrBaseAddrHealth, healthOffsets);




	bool bAmmoRifle = false, bHealth = false, bRecoil = false;
	bool running = true;

	while (running)
	{

		DWORD exitCode = 0;
		if (!GetExitCodeProcess(hProcess, &exitCode))
			running = false; // error or invalid handle

		if (exitCode != STILL_ACTIVE)
		{
			running = false; // process has exited
		}

		if (GetAsyncKeyState(VK_F1) & 1) // toggle ammo rifle hack
		{
			bAmmoRifle = !bAmmoRifle;
			printf("Turning rile ammo hack %s\n", bAmmoRifle ? "on" : "off");
		}

		if (GetAsyncKeyState(VK_F2) & 1) // toggle health hack
		{
			bHealth = !bHealth;
			printf("Turning health hack %s\n", bHealth ? "on" : "off");
		}

		if (GetAsyncKeyState(VK_F3) & 1) // exit hack
		{
			bRecoil = !bRecoil;
			printf("Turning recoil hack %s\n", bRecoil ? "on" : "off");

			if (bRecoil)
			{
				mem::PatchInternal((BYTE*)(moduleBase + 0xC8BA0), (BYTE*)"\xC2\x08\x00", 3); // switch top of recoil function to ret 08 00
			}
			else
			{
				mem::PatchInternal((BYTE*)(moduleBase + 0xC8BA0), (BYTE*)"\x83\xEC\x28", 3); // switch recoil function call back
			}
		}





		if (bAmmoRifle)
		{
			int newAmmoValue = 20;
			//WriteProcessMemory(hProcess, (BYTE*)ammoAddr, &newAmmoValue, sizeof(newAmmoValue), 0);

			*(int*)ammoAddr = newAmmoValue;
		}

		if (bHealth)
		{
			int newHealthValue = 999;
			//WriteProcessMemory(hProcess, (BYTE*)healthAddr, &newHealthValue, sizeof(newHealthValue), 0);

			*(int*)healthAddr = newHealthValue;
		}

		Sleep(20);

	}

	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}


bool APIENTRY DllMain(HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr);
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}