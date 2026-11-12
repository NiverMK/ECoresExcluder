#pragma once

#include "defines.h"

#if ENABLE_LOGGING

#include <string>
#include <fstream>
#include <format>
#include <functional>
#include <chrono>

#if _MSC_VER 
#include <windows.h>
#endif

class Logger
{
public:
	~Logger();

	static Logger& Get();

	void Init();
	void DeInit();

	template<typename... Args>
	void PrintLog(
		const std::wstring_view& _firstMessage,
		const std::wstring_view& _secondMessage,
		Args&&... args);

	void PrintLog(const std::wstring& _str);

private:
#if ENABLE_FILE_LOGGING
	std::wofstream logFileStream;
#endif

#if ENABLE_CONSOLE_LOGGING
#if _MSC_VER
	bool consoleAllocated = false;
#endif
	bool consoleInited = false;
#endif
};

template<typename... Args>
void Logger::PrintLog(
	const std::wstring_view& _firstMessage,
	const std::wstring_view& _secondMessage,
	Args&&... args)
{
	std::wstring message;
	message.reserve(_firstMessage.length() + _secondMessage.length() + 1);
	message.append(_firstMessage.begin(), _firstMessage.end());
	message.append(_secondMessage.begin(), _secondMessage.end());
	message.append(L"\n");

	const std::wstring output = std::vformat(message, std::make_wformat_args(args...));

	PrintLog(output);
}

enum class LogLevel
{
	Error = 0,
	Warning,
	Info
};

constexpr std::wstring_view LogLevelToString(const LogLevel _logLevel)
{
	switch (_logLevel)
	{
		case LogLevel::Error:
		{
			return L"Error  ";
		}
		case LogLevel::Warning:
		{
			return L"Warning";
		}
		case LogLevel::Info:
		{
			return L"Info   ";
		}
	}

	return L"Info   ";
}
#endif

#if ENABLE_LOGGING
#define LOG_LEVEL								LogLevel::Info

#define INIT_LOGGING()							Logger::Get().Init();
#define DEINIT_LOGGING()						Logger::Get().DeInit();

#define VERIFY_LOG_LEVEL(logLevel)				if constexpr (logLevel <= LOG_LEVEL)	

#define LOG(logLevel, message, ...)				VERIFY_LOG_LEVEL(logLevel) Logger::Get().PrintLog(L"{} | {} | ", message, std::chrono::system_clock::now(), LogLevelToString(logLevel), __VA_ARGS__)
#define LOG_FUNC(logLevel, message, ...)		VERIFY_LOG_LEVEL(logLevel) Logger::Get().PrintLog(L"{} | {} | {} | ", message, std::chrono::system_clock::now(), LogLevelToString(logLevel), __FUNCTIONW__, __VA_ARGS__)

#if _MSC_VER 
#define LOG_WIN_ERROR(logLevel, message, ...)	VERIFY_LOG_LEVEL(logLevel) Logger::Get().PrintLog(L"{} | {} | {} | (Error {}) | ", message, std::chrono::system_clock::now(), LogLevelToString(logLevel), __FUNCTIONW__, GetLastError(), __VA_ARGS__)
#endif

#else
#define INIT_LOGGING()
#define DEINIT_LOGGING()

#define LOG(logLevel, message, ...)				
#define LOG_FUNC(logLevel, message, ...)	

#if _MSC_VER 
#define LOG_WIN_ERROR(logLevel, message, ...)	
#endif
#endif