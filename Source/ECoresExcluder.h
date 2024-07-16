#pragma once

#include <vector>

class ECoresExcluder
{
public:
	static void SetProcessAffinity();

protected:
	static uint64_t GetOnesCount(const uint64_t _mask);
	static std::vector<uint64_t> GetProcessorsEfficiencyMasks();
};