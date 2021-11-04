#pragma once

#include "include/sys_io.h"
#include "generic.h"

#include <vector>
#include <fstream>
#include <filesystem>


namespace etm
{
	struct Entry
	{
		static inline const uint16_t HEADER_SIZE{ 0x40 };

		le_uint32_t size, string_length;
		std::string filename;
		ByteArray data;

		void load(std::istream& stream);
		void save(std::ostream& stream);
		void extract(const std::string& path);
	};

	class EtmFile
	{
		std::vector<Entry> entries;

	public:
		EtmFile() = default;
		EtmFile(const std::string& path);

		void loadFromFile(const std::string& path);
		void saveAll(const std::string& path);
	};
}
