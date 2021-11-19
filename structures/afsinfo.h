#pragma once

#include "include/sys_io.h"
#include "interface/common.h"

#include <vector>
#include <string>
#include <filesystem>

using namespace Interface;

namespace dat
{
	class AfsInfo : protected Interface::Logger
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
		AfsInfo(const Interface::Log& log) : Logger(&log) {};
		void load(const ByteArray& data, const size_t& count);

		size_t size();
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
