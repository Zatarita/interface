#include "structures/tpl.h"

namespace tpl
{
	void TplFile::load(const std::string& path)
	{
		std::ifstream stream(path, std::ios::binary);
		if (!stream.is_open())
			emit(level::error, "Unable To Open Requested File.");
		if (!validateFile(stream))
			emit(level::error, "Unable To Open Requested File. Unknown Format!");

		loadEntries(stream);
	}

	void TplFile::loadFromMemory(const ByteArray& data)
	{
		imemstream stream(data.data(), data.size());
		stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		if (!validateFile(stream))
			emit(level::error, "Unable To Open Requested File. Unknown Format!");

		loadEntries(stream);
	}

	bool TplFile::validateFile(std::istream& stream)
	{
		le_uint32_t header_guard{};
		stream >> header_guard;

		if (header_guard.cast() != HEADER_GUARD);
			return true;
		return true;
	}

	uint32_t TplFile::getEntryCount(std::istream& stream)
	{
		le_uint32_t childCount{};
		stream >> childCount;
		if (childCount > ENTRY_COUNT_GUARD) emit(level::error, "Unable To Open Requested File. Too Many Entries: " + std::to_string(childCount.cast()));
		return childCount;
	}

	void TplFile::loadEntries(std::istream& stream)
	{
		uint32_t entryCount{ getEntryCount(stream) };

		le_uint64_t FIXMEATSOMEPOINT;
		stream >> FIXMEATSOMEPOINT;

		entries.resize(entryCount);

		for (auto& entry : entries)
			entry.load(stream);
	}

	void TplFile::save(const std::string& path, bool include_mips)
	{
		auto directory_path = std::filesystem::path(path).parent_path();
		if(!directory_path.empty())
			std::filesystem::create_directories(directory_path);
		std::ofstream stream(path, std::ios::binary);

		calculateOffsets();
		writeHeader(stream);
		writeEntries(stream);
	} 

	void TplFile::calculateOffsets(bool include_mips)
	{
		std::vector<Entry*> entriesWithMipmaps;
		size_t starting_offset = TPL_HEADER_SIZE;
		size_t mipmaps_starting_offset = TPL_HEADER_SIZE + (TPL_ENTRY_SIZE * entries.size());

		for (auto& entry : entries)
		{
			starting_offset += TPL_ENTRY_SIZE + (TPL_ENTRY_SIZE * entry.getMipmapCount());
			if(entry.hasMipmaps()) entriesWithMipmaps.push_back(&entry);
		}

		for (auto& entry : entries)
		{
			entry.getTexture().setIndexOffset(starting_offset);
			starting_offset += entry.getTexture().getIndexSize();

			if (include_mips && entry.hasMipmaps())
			{
				entry.getMipmap(0).setIndexOffset(starting_offset);
				starting_offset += entry.getMipmap(0).getIndexSize();

				entry.getMipmap(1).setIndexOffset(starting_offset);
				starting_offset += entry.getMipmap(1).getIndexSize();
			}

			if (entry.getTexture().hasPalette())
			{
				entry.getTexture().setPaletteOffset(starting_offset);
				starting_offset += entry.getTexture().getPaletteSize();
			}
			
		}

		for (auto* entry : entriesWithMipmaps)
		{
			entry->getTexture().setMipmapOffset(0, mipmaps_starting_offset);
			mipmaps_starting_offset += TPL_ENTRY_SIZE;

			entry->getTexture().setMipmapOffset(1, mipmaps_starting_offset);
			mipmaps_starting_offset += TPL_ENTRY_SIZE;
		}
	}

	void TplFile::writeHeader(std::ostream& stream)
	{
		stream << le_uint32_t(HEADER_GUARD) << le_uint32_t(entries.size()) << le_uint64_t(0x10);
	}

	void TplFile::writeEntries(std::ostream& stream, bool include_mips)
	{
		for (auto& entry : entries)
			entry.saveTexture(stream, include_mips);
	}


	void TplFile::extractAll(const std::string& path, bool include_mips)
	{
		for (size_t i = 0; i < entries.size(); i++)
			extract(i, path + "/" + std::to_string(i) + ".tpl", include_mips);
	}

	void TplFile::extract(const size_t index, const std::string& path, bool include_mips)
	{
		if (index > entries.size())
			emit(level::error, "Index Exceeds Bounds Of Array");

		tpl::TplFile newFile({ entries[index] });

		newFile.save(path, include_mips);
	}

	TplFile& TplFile::operator += (TplFile& rhs)
	{
		entries.insert(entries.end(), rhs.getEntries().begin(), rhs.getEntries().end());
		return *this;
	}
}
