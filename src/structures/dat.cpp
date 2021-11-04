#include "structures/dat.h"

#include <iostream>
#include <cmath>

using namespace debug;

namespace dat
{

	DatFile::DatFile(std::string path)
	{
		loadFromFile(path);																	// Load from file is string was included in constructor
	}

	void DatFile::loadFromFile(const std::string& path)
	{
		emit(level::verbose, "\tCreating File Stream.\n");
		file = new std::ifstream(path, std::ios::ate | std::ios::binary);					// Open the file at the end

		entries.clear();																	// Clear any old entries in case we're reusing object
		
		getFileSize();																		// Get the filesize from the stream
		emit(level::verbose, "\tGetting File Size: ");
		emit(level::log, "\tInitializing File: \n");
		initializeFile();																	// Load the segment count, and get the file size.
		emit(level::log, "\tReading Header: \n");
		readHeader();																		// Read the offsets and segment types from the stream
		emit(level::log, "\tAllocating memory\n");
		allocateMemory();																	// calculate segments' size, and allocate the memory for it
	}
	void DatFile::loadFromMemory(const ByteArray& data)
	{
		emit(level::verbose, "\tCreating Memory Stream.\n");
		file = new imemstream(data.data(), data.size());										// Create a stream from the data

		entries.clear();																	// Clear any old entries in case we're reusing object

		filesize = data.size();																// Assign filesize
		emit(level::verbose, "\tGetting File Size: " + std::to_string(filesize) + '\n');
		emit(level::log, "\tInitializing File: \n");
		initializeFile();																	// Load the segment count, and get the file size.
		emit(level::log, "\tReading Header: \n");
		readHeader();																		// Read the offsets and segment types from the stream
		emit(level::log, "\tAllocating memory...\n");
		allocateMemory();																	// calculate segments' size, and allocate the memory for it
	}

	void DatFile::getFileSize()
	{
		filesize = file->tellg();															// Get the file size
		file->seekg(0);																		// Seek back to the start
	}

	void DatFile::initializeFile()
	{
		*file >> count;																		// Read the entry count from the file
		file->seekg(0x10);																	// Burn padding
		emit(level::log, "\t\tAllocating Space For " + std::to_string(count.cast()) + " Segments\n");

		entries.resize(count.cast());														// Resize the entries to match the file count.
	}

	void DatFile::readHeader()
	{
		emit(level::log, "\t\tLoading Segment Offsets...\n");
		for (Entry& entry : entries)
		{
			*file >> entry.offset;															// Read the offsets from the stream
			emit(level::verbose, "\t\t\t" + std::to_string(entry.offset.cast()));
		}
			
		emit(level::log, "\t\tLoading Segment Format...\n");
		for (Entry& entry : entries)
		{
			file->read(entry.segmentType.data(), 4);										// Read the segment types from the stream
			emit(level::verbose, "\t\t\t" + entry.segmentType);
		}
	}

	void DatFile::allocateMemory()
	{
		for (size_t i = 0; i < entries.size() - 1; i++)										// Calculate and resize the data arrays to hold enough data 
		{
			auto size = entries[i + 1].offset - entries[i].offset;
			emit(level::verbose, "\t\t\t" + std::to_string(size) + "b\n");
			entries[i].data.resize(size);													// between the start of the next offset, and the current
		}
		entries[entries.size() - 1].data.resize(filesize - entries[entries.size() - 1].offset); // Resize the last chunk to hold the data from last offset to eof
		emit(level::verbose, "\t\t\t" + std::to_string(filesize - entries[entries.size() - 1].offset) + "b\n");
	}

	const ByteArray& DatFile::get(const size_t& index)
	{
		emit(level::verbose, "\tLoading Chunk " + std::to_string(index) + " From Stream...\n");
		if (index > entries.size())
		{
			emit(level::log, "\t\tUnable To Load Entry. Exceeds Bounds Of Array!\n");
			return {};									// If the index is out of range, return empty array
		}

		Entry& curEntry = entries[index];													// Reference to current index to make things easier
		if (curEntry.data.size() == 0)
		{
			emit(level::verbose, "\t\tIgnoring Null Chunk\n");
			return entries[index].data;					// If the chunk size is 0; ignore.
		}
		if (curEntry.initialized)
		{
			emit(level::verbose, "\t\tChunk Already Loaded, Returning...\n");
			return entries[index].data;					// If the chunk has already been loaded, return existing
		}


		file->seekg(curEntry.offset.cast());												// Seek to the start of the data
		file->read((char*)entries[index].data.data(), curEntry.data.size());		    	// Read the data into the array

		curEntry.initialized = true;														// Flag this entry as already loaded from file.

		return curEntry.data;																// Return the data read from the stream
	}
	const ByteArray& DatFile::operator[] (const size_t index)
	{
		return get(index);																	// Return the data from an index
	}

	void DatFile::loadAll()
	{
		emit(level::log, "\tLoading All Segments...\n");
		for (size_t i = 0; i < entries.size(); i++)
			get(i);																			// Load each of the entries into memory from the file.
	}

	const std::string DatFile::extension(const size_t index)
	{
		if (index > entries.size()) return "";												// If index out of range, return empty string
		return std::string(entries[index].segmentType);										// Else return the char array as a string
	}

	const size_t DatFile::size()
	{
		return entries.size();																// Return how many entries are in the file
	}

	void DatFile::extract(const size_t& index, const std::string& path)
	{
		const ByteArray& data = get(index);													// Get the data to be saved
		if (data.empty()) return;															// Verify there is data to save

		std::string filename = std::to_string(index) + "." + extension(index);				// Create file name from data

		std::ofstream out(path + filename, std::ios::binary);								// Create the output stream
		out.write((const char*)data.data(), data.size());									// Write the data
		out.close();																		// Close the output stream
	}

	void DatFile::extractAll(const std::string& path)
	{
		if (!std::filesystem::exists(path))
			std::filesystem::create_directory(path);

		for (size_t i = 0; i < entries.size(); i++)
			extract(i, path + "\\");
	}

	void DatFile::append(ByteArray& data, const std::string& format)
	{
		if (format.size() < 4) return;														// 4 chars required for 4cc
		if (data.empty())	   return;														// No point appending empty data
		if (data.size() % 0x20 != 0)
			data.resize(data.size() + (20 - (data.size() % 20)));						    // Data does not match byte allignment requirements

		entries.push_back({ format, 0, data, false });										// Create the new entry
	}

	void DatFile::addDir(const std::string& path)
	{
		emit(level::log, "Loading Files From Directory: \n" + path);
		if (!std::filesystem::exists(path))
		{
			emit(level::log, "\tCould Not Find Directory. Aborting" + path);
			return;
		}
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			emit(level::log, "\t" + entry.path().string() + "\n");

			std::ifstream file(entry.path(), std::ios::ate | std::ios::binary);
			ByteArray rawData;
			rawData.resize(file.tellg());
			file.seekg(0);
			file.read((char*)rawData.data(), rawData.size());

			addNew(rawData, entry.path().extension().string().substr(1,3) + "\0");
		}
	}

	void DatFile::addNew(const ByteArray& data, const std::string& fmt)
	{
		entries.push_back({ fmt, 0, data, true });
	}

	void DatFile::save(const std::string& path)
	{
		std::ofstream out(path, std::ios::binary);

		loadAll();
		trimEmpty();																		// Remove any empty entries
		calculateOffsets();																	// Calculate the offsets for remaning entries
		writeHeader(out);																	// Write the header.
		writeData(out);																		// Write the data to file.

		out.close();
	}

	void DatFile::trimEmpty()
	{
		for (size_t i = entries.size() - 1; i > 0; i--)										// For each, from the end to begin
		{
			if (entries[i].data.empty())													// If the entry is empty
			{
				emit(level::log, "\Trimming Empty: " + std::to_string(i));
				entries.erase(entries.begin() + i);											// Erase the entry
			}
		}
		count = entries.size();																// Update entry count
	}

	void DatFile::calculateOffsets()
	{
		uint32_t startingOffset = OFFSET_START_POS + (HEADER_ENTRY_SIZE * entries.size());	// data starts after the offsets, and segments in the header
		for (Entry& entry : entries)
		{
			startingOffset = (startingOffset/0x20 + (startingOffset % 0x20 != 0)) * 0x20;
			entry.offset = startingOffset;													// Assign the offset this data will be at 
			startingOffset += entry.data.size();											// Increment the offset to include the size of this data
		}
	}

	void DatFile::writeHeader(std::ofstream& stream)
	{
		stream << count;																	// Write the segment count to file
		stream.seekp(OFFSET_START_POS);														// Seek past the padding in the start

		for (Entry& entry : entries)
			stream << entry.offset;															// Write each offset

		for (Entry& entry : entries)
		{
			if (entry.segmentType.size() < 4)
				entry.segmentType.resize(4);
			stream.write(entry.segmentType.data(), 4);										// Write each segment type
		}
	}

	void DatFile::writeData(std::ofstream& stream)
	{
		for (Entry& entry : entries)
		{
			stream.seekp(entry.offset.cast());
			stream.write((const char*)entry.data.data(), entry.data.size());				// Wite the data to the file.
		}
	}

}