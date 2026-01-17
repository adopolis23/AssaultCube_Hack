#pragma once

#include <windows.h>
#include "mem.h"

namespace hook
{
	bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len);
	BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len);

	struct Hook
	{

		Hook(BYTE* src, BYTE* dst, BYTE* gatewayPtr, uintptr_t len);
		Hook(const char* exportName, const char* modName, BYTE* dst, BYTE* gatewayPtr, uintptr_t len);

		void Enable();
		void Disable();
		void Toggle();


		// member variables

		bool bStatus{ false };

		BYTE* src{ nullptr };
		BYTE* dst{ nullptr };
		BYTE* gatewayPtr{ nullptr };
		int len{ 0 };

		BYTE* originalBytes[10]{ 0 };


	};

}