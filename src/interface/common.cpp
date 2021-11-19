#include "interface/common.h"

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

		for (size_t i = ret.size() - 1; i >= 0; i--)
			if (ret[i].empty()) ret.erase(ret.begin() + i);

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

	uint32_t allign(const uint32_t& OFFSET, const uint32_t& BLOCK_ALLIGNMENT)
	{
		return ((OFFSET / BLOCK_ALLIGNMENT) + (OFFSET % BLOCK_ALLIGNMENT != 0)) * BLOCK_ALLIGNMENT;
	}

	void validateSystemPath(const std::string& path)
	{
		if (!std::filesystem::exists(path))
			std::filesystem::create_directories(path);
	}

	void createSavePath(const std::string& path)
	{
		auto directory_path = std::filesystem::path(path).parent_path();
		if (!directory_path.empty())
			std::filesystem::create_directories(directory_path);
	}

	const std::string& to_lower(std::string& data)
	{
		std::transform(data.begin(), data.end(), data.begin(),
			[](unsigned char c) { return std::tolower(c); });
		return data;
	}

	ByteArray getFileData(const std::string& path)
	{
		ByteArray rawData;

		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (!file.is_open()) return {};

		rawData.resize(file.tellg());
		file.seekg(0);

		file.read((char*)rawData.data(), rawData.size());

		return rawData;
	}

	Logger::Logger(const Interface::Log* log) : logger(log) { }

	const Log& Logger::getLog()
	{
		return *logger;
	}

	void Logger::print(const Interface::Level& logLevel, const std::string& msg)
	{
		logger->log(logLevel, msg);
	}

	void Color::writeNormalized(std::ostream& stream)
	{
		stream << le_uint8_t(r);
		stream << le_uint8_t(g);
		stream << le_uint8_t(b);
		stream << le_uint8_t(std::min(a * 2, 0xff));
	}

	
}
