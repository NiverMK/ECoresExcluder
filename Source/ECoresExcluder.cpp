#include "pch.h"

#include "ECoresExcluder.h"
#include "Logging/Logger.h"

#include <bitset>
#include <string>

void ECoresExcluder::SetProcessAffinity()
{
	const std::vector<uint64_t> processorsEfficiencyMasks = GetProcessorsEfficiencyMasks();

	if (!processorsEfficiencyMasks.size())
	{
		LOG_FUNC(LogLevel::Error, L"Cores info is empty!");
		return;
	}

	LOG(LogLevel::Info, L"Found processor's cores:");
	for (size_t index = 0; index < processorsEfficiencyMasks.size(); index++)
	{
		std::bitset<64> bits = std::bitset<64>(processorsEfficiencyMasks[index]);
		std::wstring bits_str = bits.to_string(L'0', L'1');
		LOG(LogLevel::Info, L"Efficiency {}, cores mask {}", index, bits_str);
	}

	if (processorsEfficiencyMasks.size() == 1)
	{
		LOG_FUNC(LogLevel::Warning, L"There are no e-cores!");
		return;
	}

	DWORD_PTR processAffinityMask, systemAffinityMask;
	if (!GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask))
	{
		LOG_WIN_ERROR(LogLevel::Error, L"Can't get process affinity mask!");
	}

	if (processAffinityMask != processorsEfficiencyMasks.back())
	{
		if (!SetProcessAffinityMask(GetCurrentProcess(), processorsEfficiencyMasks.back()))
		{
			LOG_WIN_ERROR(LogLevel::Error, L"Can't set process affinity mask!");
		}
		else
		{
			std::bitset<64> bits_prev = std::bitset<64>(processAffinityMask);
			std::bitset<64> bits_new = std::bitset<64>(processorsEfficiencyMasks.back());
			std::wstring bits_prev_str = bits_prev.to_string(L'0', L'1');
			std::wstring bits_new_str = bits_new.to_string(L'0', L'1');
			LOG(LogLevel::Info, L"Process affinity mask was changed from {} to {}", bits_prev_str, bits_new_str);
		}
	}
}

std::vector<uint64_t> ECoresExcluder::GetProcessorsEfficiencyMasks()
{
	std::vector<uint64_t> processorsMasks;
	DWORD procInfoExSize = 0;

	if (!GetLogicalProcessorInformationEx(LOGICAL_PROCESSOR_RELATIONSHIP::RelationProcessorCore, nullptr, &procInfoExSize))
	{
		char* structsArray = new char[procInfoExSize];

		if (GetLogicalProcessorInformationEx(LOGICAL_PROCESSOR_RELATIONSHIP::RelationProcessorCore, reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(structsArray), &procInfoExSize))
		{
			for (DWORD index = 0; index < procInfoExSize;)
			{
				PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pCoreInfo = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(structsArray + index);
				index += pCoreInfo->Size;

				const uint8_t efficiencyClass = pCoreInfo->Processor.EfficiencyClass;

				if (const size_t totalAmount = efficiencyClass + 1; processorsMasks.size() < totalAmount)
				{
					processorsMasks.resize(totalAmount);
				}

				processorsMasks[efficiencyClass] = processorsMasks[efficiencyClass] | pCoreInfo->Processor.GroupMask->Mask;
			}
		}
		else
		{
			LOG_WIN_ERROR(LogLevel::Error, L"Can't get processor's efficiency masks!");
		}

		delete[] structsArray;
	}
	else
	{
		LOG_WIN_ERROR(LogLevel::Error, L"Can't get buffer's size!");
	}

	return processorsMasks;
}