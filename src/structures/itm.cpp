#include "structures/itm.h"

namespace itm
{
	ItmFile::ItmFile(const std::string& path, const Interface::Log& log) : Logger(&log)
	{
		loadFromFile(path);
	}

	void ItmFile::loadFromFile(const std::string& path)
	{
		std::ifstream stream(path, std::ios::binary | std::ios::ate);
		if (!stream.is_open()) return;

		getFileSize(stream);
		uint32_t count = getFileCount(stream);
		if (count)
		{
			readOffsets(stream, count);
			loadChildren(stream);
		}
	}

	void ItmFile::getFileSize(std::istream& stream)
	{
		filesize = stream.tellg();
		stream.seekg(0);
	}

	uint32_t ItmFile::getFileCount(std::istream& stream)
	{
		le_uint32_t count;
		stream >> count;

		if (count.cast() != 3) return 0;

		return count.cast();
	}

	void ItmFile::readOffsets(std::istream& stream, const uint32_t& count)
	{
		offsets.resize(count);
		for (auto& offset : offsets)
			stream >> offset;
	}

	void ItmFile::loadChildren(std::istream& stream)
	{
		loadIDs(stream);
		loadBINs(stream);
		loadTPLs(stream);
	}

	void ItmFile::resizeEntries(const uint32_t& count)
	{
		entries.resize(count, Entry(getLog()));
	}

	void ItmFile::loadIDs(std::istream& stream)
	{
		stream.seekg(offsets[0].cast());

		loadIDCount(stream);
		resizeEntries(IDCount.cast());
		loadAllIDs(stream);
	}

	void ItmFile::loadIDCount(std::istream& stream)
	{
		stream >> IDCount;
	}

	void ItmFile::loadAllIDs(std::istream& stream)
	{
		le_uint32_t padding;	// clobber register
		for (auto& entry : entries)
			stream >> entry.id >> padding;
	}

	void ItmFile::loadBINs(std::istream& stream)
	{
		stream.seekg(offsets[1].cast());

		loadBinCount(stream);
		loadBinOffsets(stream);
		loadAllBins(stream);
	}

	void ItmFile::loadBinCount(std::istream& stream)
	{
		stream >> BINCount;
		if (BINCount != IDCount)
			throw std::runtime_error("File May Be Corrupt! Child Count Mismatch.");
	}

	void ItmFile::loadBinOffsets(std::istream& stream)
	{
		for (auto& entry : entries)
			stream >> entry.binOffset;
	}

	void ItmFile::loadAllBins(std::istream& stream)
	{
		for (uint32_t i = 0; i < BINCount.cast(); i++)
		{
			if (i == BINCount.cast() - 1)
				entries[i].bin.resize(offsets[2] - entries[i].binOffset.cast());
			else
				entries[i].bin.resize(entries[i + 1].binOffset.cast() - entries[i].binOffset.cast());

			stream.seekg(offsets[1] + entries[i].binOffset.cast());
			stream.read((char*)entries[i].bin.data(), entries[i].bin.size());
		}
	}

	void ItmFile::loadTPLs(std::istream& stream)
	{
		stream.seekg(offsets[2].cast());

		loadTplCount(stream);
		loadTplOffsets(stream);
		loadAllTpls(stream);
	}

	void ItmFile::loadTplCount(std::istream& stream)
	{
		stream >> TPLCount;
		if (TPLCount != IDCount)
			throw std::runtime_error("File May Be Corrupt! Child Count Mismatch.");
	}

	void ItmFile::loadTplOffsets(std::istream& stream)
	{
		for (auto& entry : entries)
			stream >> entry.tplOffset;
	}

	void ItmFile::loadAllTpls(std::istream& stream)
	{
		for (uint32_t i = 0; i < TPLCount.cast(); i++)
		{
			ByteArray data;
			if (i == TPLCount.cast() - 1)
				data.resize(filesize - entries[i].tplOffset.cast());
			else
				data.resize(entries[i + 1].tplOffset.cast() - entries[i].tplOffset.cast());

			stream.seekg(offsets[2] + entries[i].tplOffset.cast());
			stream.read((char*)data.data(), data.size());

			entries[i].tpl.loadFromMemory(data);
		}
	}

	void ItmFile::extractTpls(const std::string& path)
	{
		auto directory_path = std::filesystem::path(path).parent_path();
		if (!directory_path.empty())
			std::filesystem::create_directories(directory_path);
		for (int i = 0; i < entries.size(); i++)
			entries[i].tpl.save(path + "/" + std::to_string(entries[i].id) + ".TPL");
	}

	void ItmFile::extractBins(const std::string& path)
	{
		if (!std::filesystem::exists(path))
			std::filesystem::create_directory(path);
		for (int i = 0; i < entries.size(); i++)
		{
			std::ofstream stream(path + "/" + std::to_string(entries[i].id) + ".bin", std::ios::binary);
			stream.write((char*)entries[i].bin.data(), entries[i].bin.size());
		}
	}
}
