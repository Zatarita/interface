#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>

#include "interface/log.h"
#include "tga.h"
#include "sys_io.h"
#include "argParser.h"
#include "thread_pool.hpp"


static inline std::ostream& operator<< (std::ostream& stream, tga::Header& rhs)
{
	stream.write((char*)&rhs.idLength, 3);
	stream << le_uint16_t(rhs.colormapOrigin) << le_uint16_t(rhs.colormapLength);
	stream.write((char*)&rhs.colormapDepth, 1);
	stream << le_uint16_t(rhs.xOrigin) << le_uint16_t(rhs.yOrigin) << le_uint16_t(rhs.width) << le_uint16_t(rhs.height);
	stream.write((char*)&rhs.bitsPerPixel, 2);
	return stream;
}

namespace Interface
{
	std::vector<std::string> getSequentialNumericFilenames(const std::string& path);
	size_t					 getHighestFileNumber		  (const std::string& path);

	class Logger
	{
	protected:
		const Interface::Log* logger;

	public:
		Logger(const Interface::Log* log);

		const Log& getLog();
		void print(const Interface::Level& logLevel, const std::string& msg);							// Wrapper class to hide logger
	};

	struct Color
	{
		unsigned char r{}, g{}, b{}, a{};

		void writeNormalized(std::ostream& stream);
	};

	static std::istream& operator >> (std::istream& stream, Color& clr)
	{
		stream.read((char*)&clr.b, 1);
		stream.read((char*)&clr.g, 1);
		stream.read((char*)&clr.r, 1);
		stream.read((char*)&clr.a, 1);
		return stream;
	}

	static std::ostream& operator << (std::ostream& stream, Color& clr)
	{
		stream.write((char*)&clr.b, 1);
		stream.write((char*)&clr.g, 1);
		stream.write((char*)&clr.r, 1);
		stream.write((char*)&clr.a, 1);
		return stream;
	}

	ByteArray getFileData(const std::string& path);
	uint32_t allign(const uint32_t& starting_offset, const uint32_t& BLOCK_ALLIGNMENT);

	void validateSystemPath(const std::string& path);
	void createSavePath(const std::string& path);

	const std::string& to_lower(std::string&);
	
	template<class numeric_t>
	static inline std::string to_hex(const numeric_t& value)
	{
		std::string ret;
		static const std::string hex_map[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f" };

		const char* raw_data = reinterpret_cast<const char*>(&value);
		for (size_t i = sizeof(numeric_t); i > 0; i--)
			ret += hex_map[ (raw_data[i - 1] >> 4) & 0xf ] + hex_map[ raw_data[i - 1] & 0x0f ];

		int pos = static_cast<int>(ret.find_first_not_of("0"));
		if (pos > 0)
			return "0x" + ret.substr(pos);
		else
			return "0x0";
	}

	static std::string StringToLower(const std::string& string)
	{
		std::string ret;
		for (const char& character : string)
			ret.push_back(std::tolower(character));
		return ret;
	}
	
	static void RecursiveExtract(const ArgParser& parser, const Log& output, const std::string& extension, void (*Extraction)(const std::filesystem::path&, const std::filesystem::path&, const Log&))
	{
		thread_pool pool;
		std::string directory;

		if (auto dir = parser.arg(0))
			directory = *dir;
		else
			directory = std::filesystem::current_path().string();

		output.log(Level::LOG, "Recursive Extracting All '" + StringToLower(extension.substr(1)) + "'s In Directory: " + directory);
		output.setLevel(Level::SILENT);

		for (auto& file : std::filesystem::recursive_directory_iterator(directory))
		{
			if (!std::filesystem::is_regular_file(file)) continue;
			if (StringToLower(file.path().extension().string()) != StringToLower(extension)) continue;

			pool.submit(Extraction, file.path(), file.path().parent_path().string() + "/" + file.path().filename().stem().string() + "-" + StringToLower(extension.substr(1)), output);
		}

		pool.wait_for_tasks();
		output.setLevel(Level::LOG);
		output.log(Level::LOG, "Done!");
	}
}
