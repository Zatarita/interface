#include "interface/generic.h"

namespace Interface
{
	std::vector<std::string> getSequentialNumericFilenames(const std::string& path)
	{
		std::vector<std::string> ret;
		ret.resize(getHighestFileNumber(path));

		size_t index{ 0 };
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			index = std::stoi(entry.path().stem().string());
			ret[index] = entry.path().string();
		}

		return ret;
	}

	size_t getHighestFileNumber(const std::string& path)
	{
		size_t ret{ 0 };
		size_t comparitor{ 0 };

		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			comparitor = std::stoi(entry.path().stem().string());
			if (ret < comparitor) ret = comparitor;
		}
		return ret + 1;
	}
}
