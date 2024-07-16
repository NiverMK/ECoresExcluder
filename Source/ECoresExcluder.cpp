#include "pch.h"

#include "ECoresExcluder.h"
#include "DebugLibrary.h"

void ECoresExcluder::SetProcessAffinity()
{
	const std::vector<uint64_t> processorsEfficiencyMasks = GetProcessorsEfficiencyMasks();

	if (!processorsEfficiencyMasks.size())
	{
		ECE_LOG_FUNC(L"Cores info is empty!");
		return;
	}

	ECE_LOG(L"Found processor's cores:");
	for (size_t index = 0; index < processorsEfficiencyMasks.size(); index++)
	{
		ECE_LOG(L"Efficiency %I64u, cores mask %I64u", index, processorsEfficiencyMasks[index]);
	}

	if (processorsEfficiencyMasks.size() == 1)
	{
		ECE_LOG_FUNC(L"There are no e-cores!");
		return;
	}

	/* update process affinity mask to block execution of threads without affinity mask on e-cores */
	DWORD_PTR processAffinityMask, systemAffinityMask;
	if (!GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask))
	{
		ECE_LOG_WIN_ERROR(L"Can't get process affinity mask!");
	}

	if (processAffinityMask != processorsEfficiencyMasks.back())
	{
		if (!SetProcessAffinityMask(GetCurrentProcess(), processorsEfficiencyMasks.back()))
		{
			ECE_LOG_WIN_ERROR(L"Can't set process affinity mask!");
		}
		else
		{
			ECE_LOG(L"Process affinity mask was changed from %I64u to %I64u", processAffinityMask, processorsEfficiencyMasks.back());
		}
	}
}

uint64_t ECoresExcluder::GetOnesCount(const uint64_t _mask)
{
	uint64_t pCoresAmount = _mask;
	pCoresAmount = (pCoresAmount & 0x5555555555555555) + ((pCoresAmount >> 1) & 0x5555555555555555);
	pCoresAmount = (pCoresAmount & 0x3333333333333333) + ((pCoresAmount >> 2) & 0x3333333333333333);
	pCoresAmount = (pCoresAmount & 0x0f0f0f0f0f0f0f0f) + ((pCoresAmount >> 4) & 0x0f0f0f0f0f0f0f0f);
	pCoresAmount = (pCoresAmount & 0x00ff00ff00ff00ff) + ((pCoresAmount >> 8) & 0x00ff00ff00ff00ff);
	pCoresAmount = (pCoresAmount & 0x0000ffff0000ffff) + ((pCoresAmount >> 16) & 0x0000ffff0000ffff);
	pCoresAmount = (pCoresAmount & 0x00000000ffffffff) + ((pCoresAmount >> 32) & 0x00000000ffffffff);

	return pCoresAmount;
}

std::vector<uint64_t> ECoresExcluder::GetProcessorsEfficiencyMasks()
{
	std::vector<uint64_t> processorsMasks;

	SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX procInfoEx;
	DWORD procInfoExSize = sizeof(procInfoEx);

	if (!GetLogicalProcessorInformationEx(LOGICAL_PROCESSOR_RELATIONSHIP::RelationProcessorCore, &procInfoEx, &procInfoExSize))
	{
		char* structsArray = new char[procInfoExSize];

		if (GetLogicalProcessorInformationEx(LOGICAL_PROCESSOR_RELATIONSHIP::RelationProcessorCore, reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(structsArray), &procInfoExSize))
		{
			for (DWORD index = 0; index < procInfoExSize;)
			{
				PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pCoreInfo = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(structsArray + index);
				index += pCoreInfo->Size;

				uint8_t efficiencyClass = pCoreInfo->Processor.EfficiencyClass;

				if (processorsMasks.size() < pCoreInfo->Processor.EfficiencyClass + 1)
				{
					const size_t amountToInsert = (pCoreInfo->Processor.EfficiencyClass + 1) - processorsMasks.size();
					processorsMasks.resize(amountToInsert);
				}

				processorsMasks[efficiencyClass] = processorsMasks[efficiencyClass] | pCoreInfo->Processor.GroupMask->Mask;
			}
		}
		else
		{
			ECE_LOG_WIN_ERROR(L"Can't get processor's efficiency masks!");
		}

		delete[] structsArray;
	}
	else
	{
		processorsMasks.emplace_back(procInfoEx.Processor.GroupMask->Mask);
	}

	return processorsMasks;
}