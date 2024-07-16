#pragma once

#include <windows.h>
#include <iostream>

#define ENABLE_LOGGING _DEBUG

#if ENABLE_LOGGING
#define ECE_LOG(message, ...)			wprintf(message, __VA_ARGS__);\
										std::wcout << std::endl;

#define ECE_LOG_FUNC(message, ...)		std::wcout << __FUNCTIONW__ << L": ";\
										wprintf(message, __VA_ARGS__);\
										std::wcout << std::endl;

#define ECE_LOG_WIN_ERROR(message, ...)	std::wcout << __FUNCTIONW__ << L" (Error " << GetLastError() << L"): ";\
										wprintf(message, __VA_ARGS__); std::wcout << std::endl;
#else
#define ECE_LOG(message, ...)			;
#define ECE_LOG_FUNC(message, ...)		;
#define ECE_LOG_WIN_ERROR(message, ...)	;
#endif

namespace DebugLibrary
{
	void CreateConsole();
}