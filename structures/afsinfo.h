#pragma once

#include "include/sys_io.h"

#include <vector>
#include <string>
#include <filesystem>

namespace dat
{
	class AfsInfo
	{
		struct Entry
		{
			std::string filename{};
			le_uint32_t size{};
		};

		std::vector<Entry> entries;

		std::string readString(std::istream& stream);

		void clearMemory();
		void allocateMemory(const size_t& count);

		void readEntries(std::istream& stream);
		void readStrings(std::istream& stream);
		void readSizes(std::istream& stream);

	public:
		void load(const ByteArray& data, const size_t& count);

		const size_t& size();
		Entry& operator[] (const size_t& index);

		std::vector<std::string> getFilePaths();
		const std::vector<Entry>& getEntries();

		void build(const std::string& path);
		size_t calculateSize();

		ByteArray compile();
		void writeStrings(std::ostream& stream);
		void writeSizes(std::ostream& stream);
	};
}
