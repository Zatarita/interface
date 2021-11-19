#pragma once

#include "include/sys_io.h"
#include "interface/common.h"

#include <vector>
#include <fstream>
#include <filesystem>

using namespace Interface;


namespace etm
{
	class Entry : protected Interface::Logger
	{
		static inline const uint16_t HEADER_SIZE{ 0x40 };

		std::string filename{};
		ByteArray data{};

		// Loading Functions
		void loadEntryHeader(std::istream& stream);
		void allocateMemory(const size_t& string_length, const size_t& data_size);
		void loadString(std::istream& stream);
		void loadData(std::istream& stream);

		// Saving Function
		void writeEntryHeader(std::ostream& stream);
		void writeFilename(std::ostream& stream);
		void writeRawdata(std::ostream& stream);
	public:
		Entry(const Interface::Log& log) : Logger(&log) {}
		Entry(const ByteArray& rawData, const std::string& filename, const Interface::Log& log);

		void load(std::istream& stream);
		void save(std::ostream& stream);
		void extract(const std::string& path);
	};

	class EtmFile : protected Interface::Logger
	{
		std::vector<Entry> entries;

		void readHeader(std::istream& stream);
		void allocateMemory(const size_t&);
		void loadEntries(std::istream& stream);

		void writeHeader(std::ostream& stream);
		void writeEntries(std::ostream& stream);
	public:
		EtmFile(const Interface::Log& log) : Logger(&log) {}
		EtmFile(const std::string& path, const Interface::Log& log);

		void loadFromFile(const std::string& path);
		void saveToFile(const std::string& path);
		void addDir(const std::string& path);
		void extractAll(const std::string& path);

		void addEntry(ByteArray rawData, std::string filename);
	};
}
