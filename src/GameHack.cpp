#include <iostream>

#include "src/process.h"

int main()
{
    PrintProcessList();

	DWORD pid = GetProcessIdByName(L"ac_client.exe");

	uintptr_t moduleBase = GetModuleBaseAddress(pid, L"ac_client.exe");

	// get handle to process (requires admin)
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	// get base address of ptr chain
	uintptr_t dynamicPtrBaseAddrAmmoAr = moduleBase + 0x00183828;
	uintptr_t dynamicPtrBaseAddrHealth = moduleBase + 0x0017E254;

	// find the ammo address in this instance of the game
	std::vector<unsigned int> ammoOffsets = { 0x8, 0x1C0, 0x324 };
	uintptr_t ammoAddr = FindDMAAddress(hProcess, dynamicPtrBaseAddrAmmoAr, ammoOffsets);

	// find the health address in this instance of the game
	std::vector<unsigned int> healthOffsets = { 0xEC };
	uintptr_t healthAddr = FindDMAAddress(hProcess, dynamicPtrBaseAddrHealth, healthOffsets);




	bool bAmmoRifle = false, bHealth = false;
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





		if (bAmmoRifle)
		{
			int newAmmoValue = 20;
			WriteProcessMemory(hProcess, (BYTE*)ammoAddr, &newAmmoValue, sizeof(newAmmoValue), 0);
		}

		if (bHealth)
		{
			int newHealthValue = 999;
			WriteProcessMemory(hProcess, (BYTE*)healthAddr, &newHealthValue, sizeof(newHealthValue), 0);
		}

		Sleep(20);

	}

    return 0;
}

