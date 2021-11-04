#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "include/sys_io.h"
#include "tpl.h"
#include "bin.h"


namespace itm
{
	struct Entry
	{
		le_uint32_t id;
		le_int32_t binOffset, tplOffset;
		ByteArray bin;		// todo -> make bin files once defined
		tpl::TplFile tpl;
	};

	class ItmFile
	{
		le_uint32_t IDCount, BINCount, TPLCount;

		std::vector<le_uint32_t> offsets;
		std::vector<Entry> entries;

		size_t filesize;

	public:
		ItmFile() = default;
		ItmFile(const std::string& path);

		void loadFromFile(const std::string& path);
		void getFileSize(std::istream& stream);

		uint32_t getFileCount(std::istream& stream);
		void readOffsets(std::istream& stream, const uint32_t& count);
		void loadChildren(std::istream& stream);
		void resizeEntries(const uint32_t& count);

		void loadIDs(std::istream& stream);
		void loadIDCount(std::istream& stream);
		void loadAllIDs(std::istream& stream);
		void loadBINs(std::istream& stream);
		void loadBinCount(std::istream& stream);
		void loadBinOffsets(std::istream& stream);
		void loadAllBins(std::istream& stream);
		void loadTPLs(std::istream& stream);
		void loadTplCount(std::istream& stream);
		void loadTplOffsets(std::istream& stream);
		void loadAllTpls(std::istream& stream);

		void extractTpls(const std::string& path);
		void extractBins(const std::string& path);
	};
}
