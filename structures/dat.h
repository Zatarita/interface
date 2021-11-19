#pragma once

#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <string_view>
#include <filesystem>

#include "include/sys_io.h"
#include "interface/common.h"

using namespace Interface;


namespace dat
{
	template<std::endian endianness = std::endian::little>
	struct Entry
	{
		std::string segmentType = "    ";
		Scalar<uint32_t, endianness> offset{};
		ByteArray data{};
	};

	template<std::endian endianness = std::endian::little>
	class DatFile : protected Interface::Logger
	{
		const static inline uint8_t  HEADER_ENTRY_SIZE	{ sizeof(uint32_t) + sizeof(char[4]) };	// Size each entry takes up in the header. Used to calculate offsets
		const static inline uint8_t  DATA_START_OFFSET	{ 0x10 };								// Starting possition of offsets.
		const static inline uint32_t MAXIMUM_ENTRY_COUNT{ 2048 };								// Maximum Entry Count

		std::vector<Entry<endianness>> entries;

		size_t		  filesize{};

		/* Parse
		*  Pipeline for loading the raw data from stream.
		*  Returns if file parsing was successful. On failure, it is assumed the file may have swapped endianness.
		*/
		bool parse(std::ifstream& stream)
		{
			print(Level::LOG, "\tInitializing File: ");
			auto entryCount = getFileCount(stream);
			if (entryCount > MAXIMUM_ENTRY_COUNT)
			{
				print(Level::ERROR, "Entry Count Exceeds Limit: " + std::to_string(entryCount) + "/" + std::to_string(MAXIMUM_ENTRY_COUNT));
				return false;
			}


			print(Level::LOG, "\t\tAllocating Space For " + std::to_string(entryCount) + " Segments");
			entries.resize(entryCount);					// Resize entries using entryCount
			print(Level::LOG, "\tReading Header: ");
			readHeader(stream);
			print(Level::LOG, "\tAllocating memory...");
			allocateMemory(entryCount);
			print(Level::LOG, "\tLoading Entries...");
			loadAll(stream);
			return true;
		}

		/* getFileCount
		*  Reads the file count from the stream. Then progresses the stream to the position needed for the next step.
		*  Returns how many child files are contained within the dat.
		*/
		uint32_t getFileCount(std::ifstream& stream)
		{
			Scalar<uint32_t, endianness> count;
			stream >> count;
			stream.seekg(DATA_START_OFFSET);
			return count;
		}

		/* readHeader
		*  Load the header offsets, and segment formatting from stream. 
		*/
		void readHeader(std::ifstream& stream)
		{
			print(Level::LOG, "\t\tLoading Segment Offsets...");
			for (auto& entry : entries)
			{
				stream >> entry.offset;
				if(entry.offset > filesize)
					print(Level::EXCEPTION, EXCEPT_TEXT("offset: " + to_hex(entry.offset.cast<std::endian::little>()) + " Exceeds Bounds Of File."));
				else
					print(Level::VERBOSE, "\t\t\toffset: " + to_hex(entry.offset.cast<std::endian::little>()));
			}

			print(Level::LOG, "\t\tLoading Segment Format...");
			for (auto& entry : entries)
			{
				stream.read(entry.segmentType.data(), 4);
				print(Level::VERBOSE, "\t\t\t" + entry.segmentType);
			}
		}

		/* allocateMemory
		*  Resize ByteArrays to be the appropriate size to hold the data contained in each segment
		*  Size is calculated by substracting the starting offset of the next segment, from the starting offset of the current.
		*  The last chunk is calculated by the difference of the file size to the last chunk.
		*/
		void allocateMemory(const uint32_t& entryCount)
		{
			for (size_t i = 0; i < entries.size() - 1; i++)
			{
				le_uint32_t size = entries[i + 1].offset - entries[i].offset;
				if (static_cast<uint32_t>(entries[i].offset) + size > filesize)
					print(Level::EXCEPTION, EXCEPT_TEXT("offset: " + to_hex(size.cast<std::endian::little>()) + " Exceeds Bounds Of File."));
				print(Level::VERBOSE, "\t\t" + to_hex(size.cast<std::endian::little>()) + "b");
				entries[i].data.resize(size);
			}
			entries[entries.size() - 1].data.resize(filesize - entries[entries.size() - 1].offset);
			print(Level::VERBOSE, "\t\t" + to_hex( be_uint32_t( static_cast<uint32_t>( filesize - entries[entries.size() - 1].offset) ) ) + "b");
		}

		/* loadAll
		*  load() each chunk into memory.
		*/
		void loadAll(std::ifstream& stream)
		{
			print(Level::LOG, "\tLoading All Segments...");
			for (size_t i = 0; i < entries.size(); i++)
				load(i, stream);
		}

		/* load
		*  Retrieve the data for the chunk from stream.
		*/
		void load(const size_t& index, std::ifstream& stream)
		{
			print(Level::VERBOSE, "\t\tLoading Chunk " + std::to_string(index) + " From Stream...");
			if (index > entries.size())
				print(Level::EXCEPTION, EXCEPT_TEXT("Unable To Load Entry. Exceeds Bounds Of Array!"));

			stream.seekg(entries[index].offset.cast());
			stream.read((char*)entries[index].data.data(), entries[index].data.size());
		}

		/* extension
		*  returns the segment format as a file extension. If a segment format is empty, it returns a DMY extension
		*  to reserve item placement location.
		*/
		const std::string extension(const size_t index)
		{
			if (index > entries.size()) return "";
			if (entries[index].segmentType[0] == '\0')
				return "DMY";
			else
				return std::string(entries[index].segmentType);
		}

		/* calculateOffsets
		*  Given the current data calculate what the starting offset for the segment of data would be in a compiled
		*  file. This is done by calculating how many header entries there will be, by mulitplying the entry count
		*  with the header entry size, and then offset for the start of data. Then each chunk has it's offset assigned
		*  and the offset gets incremented by the chunk size. This is done sequentially for each chunk before saving.
		*/
		void calculateOffsets()
		{
			uint32_t startingOffset = DATA_START_OFFSET + (HEADER_ENTRY_SIZE * static_cast<uint32_t>(entries.size()));
			for (auto& entry : entries)
			{
				startingOffset = (startingOffset / 0x20 + (startingOffset % 0x20 != 0)) * 0x20;
				entry.offset = startingOffset;
				startingOffset += static_cast<uint32_t>(entry.data.size());
			}
		}

		/* writeHeader
		*  Prime the file with the segment count, (recalculated) segment offsets, and segment formats
		*/
		void writeHeader(std::ofstream& stream)
		{
			stream << le_uint32_t( static_cast<uint32_t>( entries.size() ) );
			stream.seekp(DATA_START_OFFSET);

			for (auto& entry : entries)
				stream << entry.offset;

			for (auto& entry : entries)
			{
				if (entry.segmentType.size() < 4)
					entry.segmentType.resize(4);
				stream.write(entry.segmentType.data(), 4);
			}
		}

		/* writeData
		*  Write each chunk sequentially after writing the header.
		*/
		void writeData(std::ofstream& stream)
		{
			for (auto& entry : entries)
			{
				stream.seekp(entry.offset.cast());
				stream.write((const char*)entry.data.data(), entry.data.size());
			}
		}

	public:
		// Base constructor
		DatFile(const Interface::Log& log) : Logger(&log) {}
		/* Constructor
		*  Create the dat file, and attempt to load from file
		*/
		DatFile(std::string path, const Interface::Log& log) : Logger(&log)
		{
			loadFromFile(path);
		}

		/* loadFromFile
		*  Create a file stream and parse the contents of the file.
		*  returns success status. On failure it is safe to assume the file may have swapped endianness than expectation.
		*/
		bool loadFromFile(const std::string& path)
		{
			print(Level::VERBOSE, "\tCreating File Stream.");
			std::ifstream file(path, std::ios::ate | std::ios::binary);
			if (!file.is_open())
				print(Level::EXCEPTION, EXCEPT_TEXT("Unable To Open Requested File: " + path));
			filesize = file.tellg();
			file.seekg(0);

			print(Level::VERBOSE, "\tFile Size: " + std::to_string(filesize));
			entries.clear();

			bool ret = parse(file);
			file.close();
			return ret;
		}

		/* loadFromMemory
		*  Similar to loadFromFile; however, this creates a memory stream and reads from a preallocated chunk of rawdata.
		*/
		bool loadFromMemory(const ByteArray& data)
		{
			print(Level::VERBOSE, "\tCreating Memory Stream.\n");
			imemstream stream(data.data(), data.size());
			filesize = data.size();

			print(Level::VERBOSE, "\tFile Size: " + std::to_string(filesize));
			entries.clear();

			return parse(stream);
		}

		/* operator[]
		*  Returns the rawdata of an entry
		*/
		const ByteArray& operator[] (const size_t index)
		{
			return entries[index].data;
		}

		/* size
		*  Returns how many entries are contained within the dat file
		*/
		const size_t size()
		{
			return entries.size();
		}

		/* extract
		*  Writes an entry to disk. The filename will be automatically generated from the index ID and segment format.
		*  These elements will be used to determine position and formatting in a rebuilt dat file
		*/
		void extract(const size_t& index, const std::string& path)
		{
			const ByteArray& data = this->operator[](index);

			std::string filename = std::to_string(index) + "." + extension(index);

			std::ofstream out(path + filename, std::ios::binary);
			out.write((const char*)data.data(), data.size());
			out.close();
		}

		/* extractAll
		*  for each segment extract() to specified location
		*/
		void extractAll(const std::string& path)
		{
			if (!std::filesystem::exists(path))
				std::filesystem::create_directory(path);

			for (size_t i = 0; i < entries.size(); i++)
				extract(i, path + "\\");
		}

		/* append
		*  add an entry to the file with a specific formatting
		*/
		void append(ByteArray& data, const std::string& format)
		{
			data.resize(Interface::allign(static_cast<uint32_t>( data.size() ), 0x20));

			entries.push_back({ format, 0, data });
		}

		/* addDir
		*  For each file on hard disk cycle through and append them to our dat file.
		*  TODO add sanity check for valid file extensions
		*/
		void addDir(const std::string& path)
		{
			print(Level::LOG, "Loading Files From Directory: " + path);
			if (!std::filesystem::exists(path))
			{
				print(Level::ERROR, "\tCould Not Find Directory: " + path);
				return;
			}
			for (const auto& entry : getSequentialNumericFilenames(path))
			{
				print(Level::LOG, "\t" + entry);

				ByteArray rawData = Interface::getFileData(entry);

				if (std::filesystem::path(entry).extension().string() == ".DMY")
					append(rawData, "\0\0\0\0");
				else
					append(rawData, std::filesystem::path(entry).extension().string().substr(1, 3) + '\0');
			}
		}

		/* save
		*  Calculates new offsets, and then formats and writes the raw data to disk.
		*/
		void save(const std::string& path)
		{
			print(Level::VERBOSE, "\tCreating File Stream.");
			std::ofstream out(path, std::ios::binary);

			print(Level::VERBOSE, "\t\tUpdating Entry Count.");
			auto count = entries.size();
			print(Level::LOG, "\tCalculating Offsets...");
			calculateOffsets();
			print(Level::LOG, "\tWriting Header.");
			writeHeader(out);
			print(Level::LOG, "\tWriting Data.");
			writeData(out);

			print(Level::VERBOSE, "\tClose File Stream.");
			out.close();
		}
	};
}

using le_DatFile = dat::DatFile<std::endian::little>;
using be_DatFile = dat::DatFile<std::endian::big>;