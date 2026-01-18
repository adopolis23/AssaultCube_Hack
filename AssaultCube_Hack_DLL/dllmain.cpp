#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include "process.h"
#include "mem.h"
#include "hook.h"
#include "gameDefines.h"
#include "glText.h"
#include "glDraw.h"


//create template function type for wglSwapBuffers function
typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);

//create a pointer to the original wglSwapBuffers function
twglSwapBuffers wglSwapBuffersGateway;

GL::Font espFont;
const int espFontHeight = 15;
const int espFontWidth = 9;

const char* example = "ESP Box";

void Draw()
{
	HDC currentHDC = wglGetCurrentDC();

	if (!espFont.bBuilt || currentHDC != espFont.hdc)
	{
		espFont.Build(espFontHeight);
	}

	GL::SetupOrtho();

	GL::DrawOutlinedRect(300, 300, 100, 200, 2.0f, RGB::red);

	float textX = espFont.CenterText(300.0f, 100.0f, espFontWidth * strlen(example));
	float textY = 300.0f - espFontHeight;

	espFont.Print(textX, textY, RGB::green, "%s", example);

	GL::RestoreGL();
}

//hooked wglSwapBuffers function
BOOL __stdcall hkwglSwapBuffers(HDC hDc)
{
	//printf("Hooked\n");

	Draw();

	//call original function
	return wglSwapBuffersGateway(hDc);
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



	hook::Hook SwapBufferHook((BYTE*)GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers"), (BYTE*)hkwglSwapBuffers, (BYTE*)&wglSwapBuffersGateway, 5);
	SwapBufferHook.Enable();



	DWORD pid = GetProcessIdByName(L"ac_client.exe");
	uintptr_t moduleBase = GetModuleBaseAddress(pid, L"ac_client.exe");

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

		if (GetAsyncKeyState(VK_DELETE) & 1)
		{
			running = !running;
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