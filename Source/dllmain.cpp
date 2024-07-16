// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "ECoresExcluder.h"
#include "DebugLibrary.h"

namespace
{
	HANDLE hTimer = NULL;
	bool wasAffinityChanged = false;
}

VOID CALLBACK TimerCallback(PVOID lpParameter, BOOLEAN reserved)
{
	ECoresExcluder::SetProcessAffinity();
	wasAffinityChanged = true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DebugLibrary::CreateConsole();
		break;
	case DLL_THREAD_ATTACH:
	{
		if (!wasAffinityChanged && !hTimer)
		{
			/* don't try to change process affinity mask at startup, it may cause app crash */
			const DWORD dueTime = 5000;
			
			/* Using CreateTimerQueueTimer instead of SetTimer in DLL because SetTimer creates a new thread */
			if (!CreateTimerQueueTimer(&hTimer, NULL, TimerCallback, NULL, dueTime, 0, 0))
			{
				ECE_LOG_WIN_ERROR(L"CreateTimerQueueTimer was failed!");
			}
		}
	}
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

