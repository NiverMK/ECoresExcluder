#include "pch.h"
#include "DebugLibrary.h"

#include <fstream>

namespace
{
	bool consoleWasCreated = false;
}

void DebugLibrary::CreateConsole()
{
#if ENABLE_LOGGING
	if (consoleWasCreated)
	{
		return;
	}

	if (AllocConsole())
	{
		FILE* dummyFile;
		freopen_s(&dummyFile, "CONOUT$", "wt", stdout);
		freopen_s(&dummyFile, "CONIN$", "rt", stdin);
		freopen_s(&dummyFile, "nul", "w", stderr);
		SetConsoleTitle(L"Debug Console");
		std::ios::sync_with_stdio(true);

		consoleWasCreated = true;
	}
#endif
}
