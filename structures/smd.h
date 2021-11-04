#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>

#include "include/sys_io.h"
#include "tpl.h"
#include "bin.h"
#include "interface/log.h"

using namespace debug;

namespace smd
{
	struct Entry
	{
		le_uint16_t id{};
		le_uint8_t unknown_id{};
		le_uint8_t model_id{};
		ByteArray  unknown{ 0x3c, (std::byte)0 };		// prolly transform matrix

		le_uint32_t offset{};
		ByteArray rawdata;			// Replace with bin::BinFile

		void load(std::istream& stream);
		void save(std::ostream& stream);
	};

	class SmdFile
	{
		const static inline uint32_t SMD_HEADER_GUARD{ 0x40 };

		size_t filesize;

		le_uint16_t header_guard{ 0x0 };
		le_uint16_t model_count{};
		le_uint16_t entry_count{};
		le_uint32_t	model_offset{};
		le_uint32_t texture_offset{};
		le_uint32_t padding{};

		tpl::TplFile Tpl;

		std::vector<Entry> entries;

	public:
		SmdFile() = default;
		SmdFile(const std::string& path);
		
		void loadFromFile	 (const std::string& path);
		void loadFromMemory	 (const ByteArray& data);
		void loadFromStream	 (std::istream& stream);
		bool readHeader		 (std::istream& stream);
		void findHighestIndex();
		void readEntryHeaders(std::istream& stream);
		void readOffsets	 (std::istream& stream);
		void readBin		 (std::istream& stream);
		void readTpl		 (std::istream& stream);

		void extract(const size_t& index, const std::string& path);
		void extract(const std::string& path);

		void save(const std::string& path);

		const size_t& size() { return entries.size(); }

		const tpl::TplFile& getTpl();
		void saveTpl(const std::string& path);
		void extractTpl(const std::string& path);
	};
}

