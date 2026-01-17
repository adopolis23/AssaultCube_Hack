#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include "process.h"
#include "mem.h"
#include "hook.h"
#include "gameDefines.h"



//create template function type for wglSwapBuffers function
typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);

//create a pointer to the original wglSwapBuffers function
twglSwapBuffers owglSwapBuffers;

BOOL __stdcall hkwglSwapBuffers(HDC hDc)
{
	printf("Hooked\n");

	//call original function
	return owglSwapBuffers(hDc);
}



DWORD WINAPI HackThread(HMODULE hModule)
{
	//create console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);

	if (!f)
	{
		return 1; // error
	}

	//PrintProcessList();

	owglSwapBuffers = (twglSwapBuffers)GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");
	owglSwapBuffers = (twglSwapBuffers)hook::TrampHook32((BYTE*)owglSwapBuffers, (BYTE*)hkwglSwapBuffers, 5);

	DWORD pid = GetProcessIdByName(L"ac_client.exe");
	uintptr_t moduleBase = GetModuleBaseAddress(pid, L"ac_client.exe");

	// get handle to process (requires admin)
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);



	// find the ammo address in this instance of the game
	uintptr_t dynamicPtrBaseAddrAmmoAr = moduleBase + 0x00183828;
	std::vector<unsigned int> ammoOffsets = { 0x8, 0x1C0, 0x324 };
	uintptr_t ammoAddr = mem::FindDMAAddressInternal(dynamicPtrBaseAddrAmmoAr, ammoOffsets);


	// this address is base address of the player object
	uintptr_t playerBaseAddress = moduleBase + 0x0017E0A8;
	PlayerEnt* localPlayer = *(PlayerEnt**)(playerBaseAddress);


	bool bAmmoRifle = false, bHealth = false, bRecoil = false, bNoClip = false, bInvis = false;
	bool running = true;

	printf("Rifle Ammo Hack -> F1\n");
	printf("Health Hack -> F2\n");
	printf("Recoil Hack -> F3\n");
	printf("NoClip Hack -> F4\n");
	printf("Invis Hack -> F5\n");

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

		if (GetAsyncKeyState(VK_F3) & 1)
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

		if (GetAsyncKeyState(VK_F4) & 1) 
		{
			bNoClip = !bNoClip;
			if (bNoClip)
			{
				localPlayer->NoClip = 4;
			}
			else
			{
				localPlayer->NoClip = 0;
			}
		}

		if (GetAsyncKeyState(VK_F5) & 1)
		{
			bInvis = !bInvis;
			if (bInvis)
			{
				localPlayer->Invis = 1;
			}
			else
			{
				localPlayer->Invis = 0;
			}
		}





		if (bAmmoRifle)
		{

			localPlayer->currWeaponPtr->clip->ammo = 999;
		}

		if (bHealth)
		{
			localPlayer->Health = 999;
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