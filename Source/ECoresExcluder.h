#pragma once

#include <vector>

class ECoresExcluder
{
public:
	static void SetProcessAffinity();

protected:
	static std::vector<uint64_t> GetProcessorsEfficiencyMasks();
};