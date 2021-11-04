#include "structures/smd.h"


namespace smd
{
	void Entry::load(std::istream& stream)
	{
		stream >> id >> unknown_id >> model_id;
		stream.read((char*)unknown.data(), unknown.size());
	}

	void Entry::save(std::ostream& stream)
	{
		stream << id, model_id;
		stream.write((char*)unknown.data(), unknown.size());
	}


	SmdFile::SmdFile(const std::string& path)
	{
		loadFromFile(path);
	}

	void SmdFile::loadFromFile(const std::string& path)
	{
		emit(level::verbose, "\tCreating File Stream.\n");
		std::ifstream stream(path, std::ios::ate + std::ios::binary);
		if (!stream.is_open()) return;

		filesize = stream.tellg();
		emit(level::verbose, "\tFile Size: " + std::to_string(filesize) + "\n");
		stream.seekg(0);

		loadFromStream(stream);

		stream.close();
	}

	void SmdFile::loadFromMemory(const ByteArray& data)
	{
		emit(level::verbose, "\tCreating Memory Stream.\n");
		imemstream stream(data.data(), data.size());

		filesize = data.size();
		emit(level::verbose, "\tFile Size: " + std::to_string(filesize) + "\n");

		loadFromStream(stream);
	}

	void SmdFile::loadFromStream(std::istream& stream)
	{
		emit(level::log, "\tReading Header.\n");
		if (!readHeader(stream))
		{
			emit(level::error, "\t\tUnable To Open SMD. File Failed Header Check\n");
			return;
		}
		emit(level::log, "\tReading Entries.\n");
		readEntryHeaders(stream);
		emit(level::log, "\tReading Offsets.\n");
		readOffsets(stream);
		emit(level::log, "\tFinding Highest Index.\n");
		findHighestIndex();
		emit(level::log, "\tReading BIN Files.\n");
		readBin(stream);
		emit(level::log, "\tReading TPL File.\n");
		readTpl(stream);
	}

	bool SmdFile::readHeader(std::istream& stream)
	{
		emit(level::verbose, "\t\tValidating Header...\n");
		stream >> header_guard;
		if (header_guard != SMD_HEADER_GUARD) return false;
		emit(level::verbose, "\t\t\tSuccess!\n\t\tLoading Into Memory\n");
		stream >> entry_count >> model_offset >> texture_offset >> padding;
		emit(level::verbose, "\t\t\tSuccess!\n\t\tLoading Into Memory");
		emit(level::verbose, "\t\t\tEntry Count: " + std::to_string(entry_count) + "\n");
		emit(level::verbose, "\t\t\tModels Offset: " + std::to_string(model_offset) + "\n");
		emit(level::verbose, "\t\t\tTPL Offset: " + std::to_string(texture_offset) + "\n");
		stream.seekg(0x40);														// Look into this. I think this is a global transform for all children
		return true;
	}

	void SmdFile::findHighestIndex()
	{
		for (auto& entry : entries)
			if (model_count < entry.id) model_count = entry.id;
		emit(level::verbose, "\t\tHighest Index: " + std::to_string(model_count) + "\n");
	}

	void SmdFile::readEntryHeaders(std::istream& stream)
	{
		emit(level::verbose, "\t\tAllocating Memory...\n\t\tInitializing Entries..");
		entries.resize(entry_count.cast());
		for (auto& entry : entries)
		{
			entry.load(stream);
			emit(level::verbose, "\t\t\tLoaded ID: " + std::to_string(entry.id) + "\n");
		}
	}

	void SmdFile::readOffsets(std::istream& stream)
	{
		emit(level::verbose, "\t\tReading Model Offsets..");
		stream.seekg(model_offset.cast());
		for (auto& entry : entries)
		{
			stream >> entry.offset;
			emit(level::verbose, "\t\t\t" + std::to_string(entry.offset) + "\n");
		}
	}

	void SmdFile::readBin(std::istream& stream)
	{
		emit(level::verbose, "\t\tReading Models..");
		for (int i = 0; i < model_count; i++)
		{
			if (i == model_count - 1)
				entries[i].rawdata.resize(texture_offset - entries[i].offset);
			else
				entries[i].rawdata.resize(entries[i + 1].offset - entries[i].offset);

			emit(level::verbose, "\t\t\t[" + std::to_string(entries[i].rawdata.size()) + "bytes] @ " +
						         std::to_string(entries[i].offset.cast() + model_offset));
			stream.seekg(entries[i].offset.cast() + model_offset);
			stream.read((char*)entries[i].rawdata.data(), entries[i].rawdata.size());
		}
	}

	void SmdFile::readTpl(std::istream& stream)
	{
		emit(level::verbose, "\t\tReading TPL..");
		stream.seekg(texture_offset.cast() + 0x10);

		emit(level::verbose, "\t\t\tGathering Rawdata..");
		ByteArray rawTplData{ filesize - (texture_offset - 0x10), (std::byte)0 };
		stream.read((char*)rawTplData.data(), rawTplData.size());
		emit(level::verbose, "\t\t\tLoading TPL From Memory..");
		Tpl.loadFromMemory(rawTplData);
	}

	void SmdFile::extract(const size_t& index, const std::string& path)
	{
		if (index > model_count) return;

		emit(level::verbose, "\t\tExtracting Index " + std::to_string(index) + " To: " + path);
		emit(level::verbose, "\t\t\tCreating Filestream");
		std::ofstream file(path, std::ios::binary);
		if (!file.is_open()) return;
		emit(level::verbose, "\t\t\t\tSuccess!");

		emit(level::verbose, "\t\t\tGetting Entry");
		Entry& current = entries[index];
		emit(level::verbose, "\t\t\tWriting Entry To Disk");
		file.write((char*)current.rawdata.data(), current.rawdata.size());

		file.close();
	}

	void SmdFile::extract(const std::string& path)
	{
		emit(level::verbose, "\t\tExtracting All Indices To: " + path);
		emit(level::verbose, "\t\t\tCreating File Path");
		if (!std::filesystem::exists(path))
			std::filesystem::create_directory(path);

		emit(level::verbose, "\t\tSaving Entries...");
		for (size_t i = 0; i < model_count; i++)
			extract(i, path + "/" + std::to_string(i) + ".bin");
	}

	void SmdFile::save(const std::string& path)
	{

	}

	const tpl::TplFile& SmdFile::getTpl()
	{
		return Tpl;
	}

	void SmdFile::saveTpl(const std::string& path)
	{
		Tpl.save(path);
	}

	void SmdFile::extractTpl(const std::string& path)
	{
		if (!std::filesystem::exists(path))
			std::filesystem::create_directory(path);
		Tpl.extractAll(path + "/");
	}
}
