#include "structures/etm.h"

namespace etm
{
	void Entry::load(std::istream& stream)
	{
		uint32_t start_pos = stream.tellg();

		stream >> size >> string_length;
		stream.seekg((int)stream.tellg() + 0x18);

		filename.resize(string_length);
		data.resize(size - HEADER_SIZE);

		stream.read(filename.data(), string_length);

		stream.seekg(start_pos + HEADER_SIZE);
		stream.read((char*)data.data(), data.size());
	}

	void Entry::save(std::ostream& stream)
	{
	}

	void Entry::extract(const std::string& path)
	{
		std::ofstream file(path + "/" + filename, std::ios::binary);
		file.write((char*)data.data(), data.size());
	}

	EtmFile::EtmFile(const std::string& path)
	{
		loadFromFile(path);
	}

	void EtmFile::loadFromFile(const std::string& path)
	{
		std::ifstream stream(path, std::ios::binary);
		if (!stream.is_open()) return;

		le_uint32_t count;
		stream >> count;

		entries.resize(count.cast());

		stream.seekg(0x20);
		for (auto& entry : entries)
			entry.load(stream);
	}

	void EtmFile::saveAll(const std::string& path)
	{
		if (!std::filesystem::exists(path))
			std::filesystem::create_directory(path);

		for (auto& entry : entries)
			entry.extract(path);
	}
}
