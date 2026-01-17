#pragma once

#include <windows.h>
#include "mem.h"

namespace hook
{
	bool Detour32(BYTE* originalFunction, BYTE* newFunction, const uintptr_t len);
	BYTE* TrampHook32(BYTE* originalFunction, BYTE* newFunction, const uintptr_t len);

	struct Hook
	{

		Hook(BYTE* originalFunction, BYTE* newFunction, BYTE* gatewayPtr, uintptr_t len);
		Hook(const char* exportName, const char* modName, BYTE* newFunction, BYTE* gatewayPtr, uintptr_t len);

		void Enable();
		void Disable();
		void Toggle();


		// member variables

		bool bStatus{ false };

		BYTE* originalFunction{ nullptr };
		BYTE* newFunction{ nullptr };
		BYTE* gatewayPtr{ nullptr };
		int len{ 0 };

		BYTE originalBytes[10]{ 0 };


	};

}