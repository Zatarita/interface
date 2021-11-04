#include "afs.h"

using namespace debug;

namespace afs
{
	AfsFile::AfsFile(const std::string& path)
	{
		loadFromFile(path);
	}

	void AfsFile::save(const size_t& index, const std::string& path, const bool& mkdirs)
	{
		if (index > file_count) return;
		if (!files[index].size) return;

		if (!std::filesystem::exists(path))
			std::filesystem::create_directory(path);

		std::string outpath;
		if (!mkdirs)
			outpath = path + "/" + file_table.entries[index].filename;
		else
		{
			std::filesystem::path dir(path + "/" + datInfo[index].filename);
			if(dir.parent_path().string() != "")
				std::filesystem::create_directories(dir.parent_path());
			if(datInfo[index].filename == "_")
				outpath = path + "/" + file_table.entries[index].filename;
			else
				outpath = path + "/" + datInfo[index].filename;
		}

		std::ofstream stream(outpath, std::ios::binary);
		stream.write((char*)files[index].rawData.data(), files[index].size);
		stream.close();
	}

	void AfsFile::save(const std::string& name, const std::string& path, const bool& mkdirs)
	{
		save(indexFromName(name), path, mkdirs);
	}

	void AfsFile::saveAll(const std::string& path, const bool& mkdirs)
	{
		for (int i = 1; i < file_count-1; i++)
			save(i, path, mkdirs);
	}

	const size_t& AfsFile::indexFromName(const std::string& name)
	{
		for (size_t i = 0; i < file_count; i++)
			if (file_table.entries[i].filename == name) return i;
		return 0xffffffff;
	}

	bool AfsFile::loadFromFile(const std::string& path)
	{
		emit(level::log, "Creating File Stream..\n");
		std::ifstream stream(path, std::ios::binary);
		if (!stream.is_open())		 return false;
		if (!validateHeader(stream)) return false;
		emit(level::log, "\tFile Open And Verified!\n");

		emit(level::log, "Parsing Contents\n");
		getFileCount(stream);
		emit(level::log, "\tAllocating Memory:\n");
		allocateHeaderMemory();
		emit(level::log, "\tReading TOC..\n");
		readEntryHeaders(stream);
		emit(level::log, "\tRetreiving File Table Metadata\n");
		readFileTableHeader(stream);
		emit(level::log, "\tAllocating Memory For File Contents:\n");
		allocateFileMemory();
		emit(level::log, "\tReading Files\n");
		readEntries(stream);
		emit(level::log, "\tLoading File Table Heirarchy\n");
		readFileTable(stream);
		emit(level::log, "\tLoading AFS Info\n");
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
		emit(level::verbose, "\tFile Count: " + std::to_string(file_count.cast()) + "\n");
	}

	void AfsFile::allocateHeaderMemory()
	{
		emit(level::verbose, "\t\tClearing Any Potential Old Entries\n");
		files.clear();
		file_table.entries.clear();

		emit(level::verbose, "\t\tResizing File Table, and TOC\n");
		files.resize(file_count);
		file_table.entries.resize(file_count);
	}

	void AfsFile::readEntryHeaders(std::istream& stream)
	{
		for (Entry& file : files)
		{
			stream >> file.offset >> file.size;
			emit(level::verbose, std::string("\t\t" + std::to_string(file.offset.cast()) + " [" + std::to_string(file.size.cast())) + "]\n");
		}
	}

	void AfsFile::readFileTableHeader(std::istream& stream)
	{
		stream >> file_table.offset >> file_table.size;
		emit(level::verbose, "\t\tFile Table: " + std::to_string(file_table.offset.cast()) + " [" + std::to_string(file_table.size.cast()) + "]\n");
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
			entry.filename.resize(FILENAME_LENGTH);
			stream.write(entry.filename.data(), entry.filename.size());
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
		file_count = datInfo.size() + 1;
	}

	void AfsFile::buildFileTable()
	{
		files[0].size = (uint32_t) datInfo.calculateSize();
		file_table.entries[0] = { {}, "afsinfo.dat", files[0].size };
		for (size_t i = 1; i < file_count; i++)
		{
			// I ignore last edit date as there is no easy way to do this right now
			std::string filename = std::filesystem::path(datInfo[i].filename).filename().string();

			file_table.entries[i] = { {}, filename, datInfo[i].size };
		}
		return;
	}

	static inline uint32_t allign(uint32_t starting_offset, uint32_t BLOCK_ALLIGNMENT)
	{
		return ((starting_offset / BLOCK_ALLIGNMENT) + (starting_offset % BLOCK_ALLIGNMENT != 0)) * BLOCK_ALLIGNMENT;
	}

	void AfsFile::calculateOffsets()
	{
		uint32_t starting_offset = ((file_count + 2) * 8);
		for (auto& entry : files)
		{
			starting_offset = allign(starting_offset, BLOCK_ALLIGNMENT);
			entry.offset = starting_offset;
			starting_offset += entry.size;
		}
		starting_offset = allign(starting_offset, BLOCK_ALLIGNMENT);
		file_table.offset = starting_offset;
		file_table.size = file_count * ENTRY_SIZE;
	}

	void AfsFile::writeFile(const std::string& path)
	{
		std::ofstream stream(path, std::ios::binary);
		if (!stream.is_open()) return;

		writeHeader(stream);
		writeEntryHeaders(stream);
		writeFileTable(stream);
		writeEntries(stream);
		padRemainingFile(stream);

		stream.close();
	}

	void AfsFile::writeHeader(std::ostream& stream)
	{
		stream.write("AFS\0", 4);
		stream << file_count;
	}

	void AfsFile::readFilesToEntries(const std::string& path)
	{
		for (int i = 1; i < file_count; i ++)
		{
			files[i].size = datInfo[i].size;
			files[i].rawData.resize(datInfo[i].size);

			std::ifstream stream(path + "/" + datInfo[i].filename, std::ios::binary);
			stream.read((char*)files[i].rawData.data(), datInfo[i].size);
			stream.close();
		}
	}

	void AfsFile::writeEntryHeaders(std::ostream& stream)
	{
		for (auto& entry : files)
			stream << entry.offset << entry.size;
	}

	void AfsFile::generateAfsInfo()
	{
		files[0].rawData = datInfo.compile();
		files[0].size = files[0].rawData.size();
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
		stream << file_table.offset << file_table.size;
		stream.seekp(file_table.offset.cast());
		file_table.write(stream);
	}

	void AfsFile::padRemainingFile(std::ostream& stream)
	{
		stream.seekp(allign(stream.tellp(), BLOCK_ALLIGNMENT) - 4);
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
}
