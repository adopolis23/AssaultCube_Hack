#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include "process.h"
#include "mem.h"
#include "hook.h"
#include "gameDefines.h"
#include "glText.h"
#include "glDraw.h"
#include "esp.h"
#include "globals.h"
#include "aimbot.h"

Globals globals;

//create template function type for wglSwapBuffers function
typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);

//create a pointer to the original wglSwapBuffers function
twglSwapBuffers wglSwapBuffersGateway;

GL::Font espFont;
const int espFontHeight = 15;
const int espFontWidth = 9;

DWORD pid = GetProcessIdByName(L"ac_client.exe");
uintptr_t moduleBase = GetModuleBaseAddress(pid, L"ac_client.exe");

ESP esp;

float g_View[16];
float g_Proj[16];

void Draw()
{
	HDC currentHDC = wglGetCurrentDC();

	if (!espFont.bBuilt || currentHDC != espFont.hdc)
	{
		espFont.Build(espFontHeight);
	}

	GL::SetupOrtho();
	esp.Draw(espFont);
	GL::RestoreGL();
}

//hooked wglSwapBuffers function
BOOL __stdcall hkwglSwapBuffers(HDC hDc)
{

	Draw();

	//call original function
	return wglSwapBuffersGateway(hDc);
}




// MATRIXMODE Hook
typedef void(__stdcall* glMatrixMode_t)(GLenum mode);
glMatrixMode_t o_glMatrixMode;
GLenum currentMatrixMode = 0;

void __stdcall hk_glMatrixMode(GLenum mode)
{
	currentMatrixMode = mode;
	o_glMatrixMode(mode);
}
// // //


// MULTMATRIXF Hook
typedef void(__stdcall* glMultMatrixf_t)(const GLfloat* m);
glMultMatrixf_t o_glMultMatrixf;

void __stdcall hk_glMultMatrixf(const GLfloat* m)
{
	if (currentMatrixMode == GL_MODELVIEW)
	{
		memcpy(g_View, m, sizeof(float) * 16);
	}
	o_glMultMatrixf(m);
}
// // //


// LOADMATRIX Hook
typedef void(__stdcall* glLoadMatrixf_t)(const GLfloat* m);
glLoadMatrixf_t o_glLoadMatrixf;

void __stdcall hk_glLoadMatrixf(const GLfloat* m)
{
	// capture projection or modelview depending on matrix mode
	if (currentMatrixMode == GL_PROJECTION)
	{
		memcpy(g_Proj, m, sizeof(float) * 16);
	}

	else if (currentMatrixMode == GL_MODELVIEW)
	{
		memcpy(g_View, m, sizeof(float) * 16);
	}

	o_glLoadMatrixf(m);
}


DWORD WINAPI HackThread(HMODULE hModule)
{
	globals.Initialize((uintptr_t)moduleBase);

	//create console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);

	if (!f)
	{
		return 1; // error
	}
	
	//hook::Hook glLoadMatrixfHook((BYTE*)GetProcAddress(GetModuleHandleA("opengl32.dll"), "glLoadMatrixf"), (BYTE*)hk_glLoadMatrixf, (BYTE*)&o_glLoadMatrixf, 5);
	//glLoadMatrixfHook.Enable();

	//hook::Hook glMatrixModeHook((BYTE*)GetProcAddress(GetModuleHandleA("opengl32.dll"), "glMatrixMode"), (BYTE*)hk_glMatrixMode, (BYTE*)&o_glMatrixMode, 5);
	//glMatrixModeHook.Enable();

	//hook::Hook glMatrixMultfHook((BYTE*)GetProcAddress(GetModuleHandleA("opengl32.dll"), "glMultMatrixf"), (BYTE*)hk_glMultMatrixf, (BYTE*)&o_glMultMatrixf, 5);
	//glMatrixMultfHook.Enable();

	hook::Hook SwapBufferHook((BYTE*)GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers"), (BYTE*)hkwglSwapBuffers, (BYTE*)&wglSwapBuffersGateway, 5);
	SwapBufferHook.Enable();




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
				globals.localPlayer->NoClip = 4;
			}
			else
			{
				globals.localPlayer->NoClip = 0;
			}
		}

		if (GetAsyncKeyState(VK_DELETE) & 1)
		{
			running = !running;
		}


		if (bAmmoRifle)
		{

			globals.localPlayer->currWeaponPtr->clip->ammo = 999;
		}

		if (bHealth)
		{
			globals.localPlayer->Health = 999;
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