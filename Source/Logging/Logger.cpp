#include "Logger.h"

#if ENABLE_LOGGING

#include <iostream>

namespace
{
	Logger logger;
}

Logger::~Logger()
{
	DeInit();
}

Logger& Logger::Get()
{
	return logger;
}

void Logger::Init()
{
#if ENABLE_CONSOLE_LOGGING
#if _MSC_VER
	if (!consoleAllocated)
	{
		consoleAllocated = AllocConsole();
	}

	if (consoleAllocated)
	{
		FILE* dummyFile;
		freopen_s(&dummyFile, "CONOUT$", "wt", stdout);
		freopen_s(&dummyFile, "CONIN$", "rt", stdin);
		freopen_s(&dummyFile, "nul", "w", stderr);
		SetConsoleTitle("Debug Console");
		consoleInited = std::ios::sync_with_stdio(true);
	}
#endif
#endif
	
#if ENABLE_FILE_LOGGING
	if (!logFileStream.is_open())
		logFileStream.open(LOG_FILENAME, std::ios_base::out);
#endif

	/* checks */
#if ENABLE_CONSOLE_LOGGING && ENABLE_FILE_LOGGING
	if (!consoleInited && logFileStream.is_open())
	{
		logFileStream << L"Can't open the console." << std::endl;
		logFileStream.flush();
	}

	if (consoleInited && !logFileStream.is_open())
	{
		std::wcout << L"Can't open log file " << LOG_FILENAME << std::endl;
	}
#endif
}

void Logger::DeInit()
{
#if ENABLE_CONSOLE_LOGGING
#if _MSC_VER
	if (consoleInited)
	{
		fclose(stdout);
		fclose(stdin);
		fclose(stderr);
	}

	if (consoleAllocated)
		FreeConsole();

	consoleAllocated = consoleInited = false;
#endif
#endif

#if ENABLE_FILE_LOGGING
	logFileStream.close();
#endif
}

void Logger::PrintLog(const std::wstring& _str)
{
#if ENABLE_CONSOLE_LOGGING
	if (consoleInited) [[likely]]
		std::wcout << _str;
#endif

#if ENABLE_FILE_LOGGING
	if (logFileStream.is_open()) [[likely]]
	{
		logFileStream << _str;
		logFileStream.flush();
	}
#endif
}

#endif