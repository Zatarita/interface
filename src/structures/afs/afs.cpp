#include "afs.h"

namespace afs
{
	AfsFile::AfsFile(const std::string& path, const Interface::Log& log) : Logger(&log), datInfo(log)
	{
		loadFromFile(path);
	}

	void AfsFile::extract(const size_t& index, const std::string& path, const bool& ignore_empty)
	{
		if (index > file_count) return;
		if (files[index].rawData.empty() && ignore_empty) return;

		std::string outpath;
		if(datInfo[index].filename == "_")
			outpath = path + "/" + file_table.entries[index].filename;
		else
			outpath = path + "/" + datInfo[index].filename;

		Interface::createSavePath(outpath);

		std::ofstream stream(outpath, std::ios::binary);
		stream.write((char*)files[index].rawData.data(), files[index].size);
		stream.close();
	}

	void AfsFile::extract(const std::string& name, const std::string& path, const bool& ignore_empty)
	{
		extract(indexFromName(name), path, ignore_empty);
	}

	void AfsFile::extractAll(const std::string& path, const bool& ignore_empty)
	{
		for (uint32_t i = 1; i < file_count - 1; i++)
			extract(i, path, ignore_empty);
	}

	size_t AfsFile::indexFromName(const std::string& name)
	{
		for (size_t i = 0; i < file_count; i++)
			if (file_table.entries[i].filename == name) return i;
		return 0xffffffff;
	}

	bool AfsFile::loadFromFile(const std::string& path)
	{
		print(Level::LOG, "Creating File Stream..");
		std::ifstream stream(path, std::ios::binary);
		if (!stream.is_open())		 return false;
		if (!validateHeader(stream)) return false;
		print(Level::LOG, "\tFile Open And Verified!");

		print(Level::LOG, "Parsing Contents");
		getFileCount(stream);
		print(Level::LOG, "\tAllocating Memory:");
		allocateHeaderMemory();
		print(Level::LOG, "\tReading TOC..");
		readEntryHeaders(stream);
		print(Level::LOG, "\tRetreiving File Table Metadata");
		readFileTableHeader(stream);
		print(Level::LOG, "\tAllocating Memory For File Contents:");
		allocateFileMemory();
		print(Level::LOG, "\tReading Files");
		readEntries(stream);
		print(Level::LOG, "\tLoading File Table Heirarchy");
		readFileTable(stream);
		print(Level::LOG, "\tLoading AFS Info");
		readDatInfo();

		return true;
	}

	bool AfsFile::validateHeader(std::istream& stream)
	{
		stream.read(signature.data(), 4);
		if (signature != SIGNATURE)
			return false;
		return true;
	}

	void AfsFile::getFileCount(std::istream& stream) 
	{
		stream >> file_count;
		print(Level::VERBOSE, "\tFile Count: " + std::to_string(file_count.cast()));
	}

	void AfsFile::allocateHeaderMemory()
	{
		print(Level::VERBOSE, "\t\tClearing Any Potential Old Entries");
		files.clear();
		file_table.entries.clear();

		print(Level::VERBOSE, "\t\tResizing File Table, and TOC");
		files.resize(file_count);
		file_table.entries.resize(file_count);
	}

	void AfsFile::readEntryHeaders(std::istream& stream)
	{
		for (Entry& file : files)
		{
			stream >> file.offset >> file.size;
			print(Level::VERBOSE, std::string("\t\t" + std::to_string(file.offset.cast()) + " [" + std::to_string(file.size.cast())) + "]");
		}
	}

	void AfsFile::readFileTableHeader(std::istream& stream)
	{
		stream >> file_table.offset >> file_table.size;
		print(Level::VERBOSE, "\t\tFile Table: " + std::to_string(file_table.offset.cast()) + " [" + std::to_string(file_table.size.cast()) + "]");
	}

	void AfsFile::allocateFileMemory()
	{
		for (Entry& file : files)
			file.rawData.resize(file.size);
	}

	void AfsFile::readEntries(std::istream& stream)
	{
		for (Entry& file : files)
		{
			stream.seekg(file.offset.cast());
			stream.read((char*)file.rawData.data(), file.size.cast());
		}
	}

	void AfsFile::readFileTable(std::istream& stream)
	{
		stream.seekg(file_table.offset.cast());
		file_table.read(stream);
	}

	void AfsFile::FileTable::read(std::istream& stream)
	{
		for (Entry& entry : entries)
		{
			entry.filename.resize(FILENAME_LENGTH);
			stream.read(entry.filename.data(), FILENAME_LENGTH);
			stream >> entry.time.year >> entry.time.month >> entry.time.day
				>> entry.time.hour >> entry.time.minute >> entry.time.second;
			stream >> entry.filesize;
		}
	}

	void AfsFile::FileTable::write(std::ostream& stream)
	{
		for (Entry& entry : entries)
		{
			if (entry.filename == "_")
			{
				stream.write(SIZE0.data(), SIZE0.size());
				continue;
			}
			else
			{
				entry.filename.resize(FILENAME_LENGTH);
				stream.write(entry.filename.data(), entry.filename.size());
			}
			stream << entry.time.year << entry.time.month << entry.time.day
				   << entry.time.hour << entry.time.minute << entry.time.second;
			stream << entry.filesize;
		}
	}

	void AfsFile::readDatInfo()
	{
		datInfo.load(files[0].rawData, file_count);
	}

	std::vector<std::string> AfsFile::getFileNames()
	{
		std::vector<std::string> filenames;
		for (auto& file : file_table.entries)
			filenames.push_back(file.filename);
		return filenames;
	}

	std::vector<std::string> AfsFile::getFilePaths()
	{
		return datInfo.getFilePaths();
	}

	void AfsFile::buildAfsInfo(const std::string path)
	{
		datInfo.build(path);
		file_count = static_cast<uint32_t>(datInfo.size());
	}

	void AfsFile::buildFileTable()
	{
		files[0].size = (uint32_t) datInfo.calculateSize();
		if(file_table.entries[0].filename.empty())
			file_table.entries[0] = { {}, "afsinfo.dat", files[0].size };
		for (size_t i = 1; i < file_count; i++)
		{
			// I ignore last edit date as there is no easy way to do this right now
			std::string filename = std::filesystem::path(datInfo[i].filename).filename().string();

			file_table.entries[i] = { {}, filename, datInfo[i].size };
		}
		return;
	}

	void AfsFile::calculateOffsets()
	{
		uint32_t starting_offset = ((file_count + 2) * 8);
		for (auto& entry : files)
		{
			starting_offset = Interface::allign(starting_offset, BLOCK_ALLIGNMENT);
			entry.offset = starting_offset;
			starting_offset += entry.size;
		}
		starting_offset = Interface::allign(starting_offset, BLOCK_ALLIGNMENT);
		file_table.offset = starting_offset;
		file_table.size = file_count * ENTRY_SIZE;
	}

	void AfsFile::writeFile(const std::string& path)
	{
		std::ofstream stream(path, std::ios::binary);
		if (!stream.is_open()) return;

		writeHeader(stream);
		writeEntryHeaders(stream);
		writeEntries(stream);
		writeFileTable(stream);
		padRemainingFile(stream);

		stream.close();
	}

	void AfsFile::writeHeader(std::ostream& stream)
	{
		stream.write("AFS\0", 4);
		stream << le_uint32_t(static_cast<uint32_t>(files.size()));
	}

	void AfsFile::readFilesToEntries(const std::string& path)
	{
		for (uint32_t i = 1; i < file_count; i ++)
		{
			files[i].size = datInfo[i].size;
			files[i].rawData.resize(datInfo[i].size);

			std::ifstream stream(path + "/" + datInfo[i].filename, std::ios::binary);
			stream.read((char*)files[i].rawData.data(), datInfo[i].size);
			stream.close();
		}
		return;
	}

	void AfsFile::writeEntryHeaders(std::ostream& stream)
	{
		for (auto& entry : files)
			stream << entry.offset << entry.size;
		stream << file_table.offset << file_table.size;
	}

	void AfsFile::generateAfsInfo()
	{
		files[0].rawData = datInfo.compile();
		files[0].size = static_cast<uint32_t>(files[0].rawData.size());
	}

	void AfsFile::writeEntries(std::ostream& stream)
	{
		for (auto& entry : files)
		{
			stream.seekp(entry.offset.cast());
			stream.write((char*)entry.rawData.data(), entry.size);
		}
	}

	void AfsFile::writeFileTable(std::ostream& stream)
	{
		stream.seekp(file_table.offset.cast());
		file_table.write(stream);
	}

	void AfsFile::padRemainingFile(std::ostream& stream)
	{
		stream.seekp( Interface::allign(static_cast<uint32_t>(stream.tellp()), BLOCK_ALLIGNMENT) - 4 );
		stream << le_int32_t{};
	}

	void AfsFile::buildAfs(const std::string& path, const std::string& out)
	{
		buildAfsInfo(path);
		allocateHeaderMemory();
		generateAfsInfo();
		readFilesToEntries(path);
		buildFileTable();
		calculateOffsets();
		writeFile(out);
	}

	void AfsFile::save(const std::string& path)
	{
		buildFileTable();
		calculateOffsets();
		writeFile(path);
	}
}
