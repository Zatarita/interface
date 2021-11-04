#pragma once

#include <vector>
#include <string>
#include <filesystem>

namespace Interface
{
	std::vector<std::string> getSequentialNumericFilenames(const std::string& path);
	size_t					 getHighestFileNumber		  (const std::string& path);
}
