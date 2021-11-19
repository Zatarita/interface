#include "structures/etm.h"

namespace etm
{
	Entry::Entry(const ByteArray& rawData, const std::string& name, const Interface::Log& log) : Logger(&log), filename(name), data(rawData) {}

	void Entry::load(std::istream& stream)
	{
		loadEntryHeader(stream);		
		loadString(stream);
		loadData(stream);
	}

	void Entry::loadEntryHeader(std::istream& stream)
	{
		print(Level::LOG, "Reading Entry Header...");
		le_uint32_t size{}, string_length{};
		stream >> size >> string_length;

		stream.ignore(0x18);

		allocateMemory(string_length.cast(), size.cast());
	}

	void Entry::allocateMemory(const size_t& string_length, const size_t& data_size)
	{
		print(Level::LOG, "Allocating Memory...");

		filename.resize(string_length);
		print(Level::VERBOSE, "  Filename Length: " + std::to_string(string_length));

		data.resize(data_size - HEADER_SIZE);
		print(Level::VERBOSE, "  Data Length: " + std::to_string(string_length));
	}

	void Entry::loadString(std::istream& stream)
	{
		stream.read(filename.data(), filename.size());
		print(Level::LOG, "Filename: " + filename);

		stream.ignore(0x20 - filename.size());
	}

	void Entry::loadData(std::istream& stream)
	{
		print(Level::LOG, "Loading Data...");
		stream.read((char*)data.data(), data.size());
	}

	void Entry::save(std::ostream& stream)
	{
		print(Level::LOG, ""); // justa new line
		writeEntryHeader(stream);
		writeFilename(stream);
		writeRawdata(stream);
	}

	void Entry::writeEntryHeader(std::ostream& stream) 
	{
		print(Level::LOG, "Writing Entry Header...");
		print(Level::VERBOSE, "  Filename Length: " + to_hex(filename.size()));
		print(Level::VERBOSE, "  Rawdata Size: " + to_hex(data.size()) + "b");
		stream << le_uint32_t(static_cast<uint32_t>(data.size())) << le_uint32_t(static_cast<uint32_t>(filename.size()));
		stream.seekp((size_t)stream.tellp() + 0x18);
	}

	void Entry::writeFilename(std::ostream& stream) 
	{
		print(Level::LOG, "Writing Filename: " + filename);
		stream.write(filename.data(), filename.size());
		stream.seekp((size_t)stream.tellp() + (0x20 - filename.size()));
	}

	void Entry::writeRawdata(std::ostream& stream) 
	{
		print(Level::LOG, "Writing Rawdata...");
		stream.write((char*)data.data(), data.size());
	}


	void Entry::extract(const std::string& path)
	{
		Interface::validateSystemPath(path);
		std::ofstream file(path + "/" + filename, std::ios::binary);

		print(Level::LOG, "Saving Entry To: " + path + "/" + filename);
		file.write((char*)data.data(), data.size());
		print(Level::VERBOSE, to_hex(data.size()) + "b Saved.");
	}

	EtmFile::EtmFile(const std::string& path, const Interface::Log& log) : Logger(&log)
	{
		loadFromFile(path);
	}

	void EtmFile::loadFromFile(const std::string& path)
	{
		std::ifstream stream(path, std::ios::binary);
		if (!stream.is_open()) return;

		readHeader(stream);
		loadEntries(stream);
	}

	void EtmFile::saveToFile(const std::string& path)
	{
		Interface::createSavePath(path);
		std::ofstream stream(path, std::ios::binary);
		if (!stream.is_open())
		{
			print(Level::ERROR, "Unable to Open Requested File: " + path);
			return;
		}

		writeHeader(stream);
		writeEntries(stream);
	}

	void EtmFile::writeHeader(std::ostream& stream)
	{
		print(Level::LOG, "Writing Header");
		stream << le_uint32_t(static_cast<uint32_t>(entries.size()));
		stream.seekp(0x20);
	}

	void EtmFile::writeEntries(std::ostream& stream)
	{
		print(Level::LOG, "Writing Entries...");
		for (auto& entry : entries)
			entry.save(stream);
	}


	void EtmFile::readHeader(std::istream& stream)
	{
		print(Level::LOG, "Reading Header...");
		le_uint32_t count;
		stream >> count;
		print(Level::VERBOSE, "  " + std::to_string(count) + " Entries In File");

		allocateMemory(count.cast());
		stream.seekg(0x20);
	}

	void EtmFile::allocateMemory(const size_t& size)
	{
		print(Level::LOG, "Allocating Memory...");
		entries.resize(size, getLog());
	}

	void EtmFile::loadEntries(std::istream& stream)
	{
		print(Level::LOG, "Loading " + std::to_string(entries.size()) + " Entries");
		for (auto& entry : entries)
			entry.load(stream);
	}

	void EtmFile::extractAll(const std::string& path)
	{
		Interface::createSavePath(path);

		print(Level::LOG, "Extracting Each Entry Into Save Directory");
		for (auto& entry : entries)
			entry.extract(path);
	}

	void EtmFile::addDir(const std::string& path)
	{
		print(Level::LOG, "Loading Files From Directory: " + path);
		if (!std::filesystem::exists(path))
		{
			print(Level::ERROR, "\tCould Not Find Directory: " + path);
			return;
		}
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			print(Level::LOG, "\t" + entry.path().string());
			addEntry(Interface::getFileData(entry.path().string()), entry.path().filename().string());
		}
	}

	void EtmFile::addEntry(ByteArray rawData, std::string filename)
	{
		if (filename == "")
		{
			print(Level::ERROR, "Unable to Add ETM Entry, Filename Invalid: " + filename);
			return;
		}
		if (rawData.empty())
		{
			print(Level::ERROR, "Unable to Add ETM Entry, Rawdata Size is 0");
			return;
		}

		filename.resize(size_t(0x20));													    // String size max is 0x20
		rawData.resize( Interface::allign( static_cast<uint32_t>(rawData.size()), 0x20 ) ); // Allign to nearest 0x20 block.

		entries.push_back( Entry( rawData, filename, getLog() ) );   // Add the entry
	}
}
