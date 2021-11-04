#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <string_view>
#include <filesystem>

#include "include/sys_io.h"
#include "generic.h"


namespace dat
{
	struct Entry
	{
		std::string segmentType = "    ";
		le_uint32_t offset{};
		ByteArray   data{};
		bool initialized{};
	};

	class DatFile
	{
		const static inline uint8_t HEADER_ENTRY_SIZE{ sizeof(uint32_t) + sizeof(char[4]) };	// Size each entry takes up in the header. Used to calculate offsets
		const static inline uint8_t OFFSET_START_POS{ 0x10 };									// Starting possition of offsets.

		le_uint32_t count{};
		std::vector<Entry> entries;

		std::istream* file;
		size_t		  filesize;

	public:
		DatFile() = default;
		DatFile(std::string path);

		void loadFromFile(const std::string& path);
		void loadFromMemory(const ByteArray& data);

		void initializeFile();
		void getFileSize();
		void readHeader();
		void allocateMemory();

		const ByteArray& get(const size_t& index);
		const ByteArray& operator[] (const size_t index);
		void loadAll();

		const std::string extension(const size_t index);
		const size_t size();

		void extract(const size_t& index, const std::string& path);
		void extractAll(const std::string& path);
		void append(ByteArray& data, const std::string& format);

		void addDir(const std::string& path);
		void addNew(const ByteArray& data, const std::string& fmt);

		void save(const std::string& path);
		void trimEmpty();
		void calculateOffsets();
		void writeHeader(std::ofstream& stream);
		void writeData(std::ofstream& stream);
	};
}