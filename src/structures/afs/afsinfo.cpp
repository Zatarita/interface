#include "structures/afsinfo.h"

namespace dat
{
	std::string AfsInfo::readString(std::istream& stream)
	{
		std::string ret(0x100, '\0');
		stream.getline(ret.data(), 0x100, 0xA);	// Explicate delim incase binary mode stream
		return ret.substr(0, ret.find(0xD));		// Remove the special characters from end
	}

	void AfsInfo::clearMemory()
	{
		entries.clear();
	}

	void AfsInfo::allocateMemory(const size_t& count)
	{
		entries.resize(count);
	}

	void AfsInfo::readEntries(std::istream& stream)
	{
		readStrings(stream);
		readSizes(stream);
	}

	void AfsInfo::readStrings(std::istream& stream)
	{
		for (Entry& entry : entries)
			entry.filename = readString(stream);
	}

	void AfsInfo::readSizes(std::istream& stream)
	{
		for (Entry& entry : entries)
			stream >> entry.size;
	}

	void AfsInfo::load(const ByteArray& data, const size_t& count)
	{
		imemstream stream(data.data(), data.size());

		clearMemory();
		allocateMemory(count);
		readEntries(stream);
	}

	size_t AfsInfo::size()
	{
		return entries.size();
	}

	AfsInfo::Entry& AfsInfo::operator[] (const size_t& index)
	{
		if (index > size())
			throw std::runtime_error("Index Exceeds The Bounds Of The Array");
		return entries[index];
	}

	std::vector<std::string> AfsInfo::getFilePaths()
	{
		std::vector<std::string> filenames;
		for (auto& file : entries)
			filenames.push_back(file.filename);
		return filenames;
	}

	const std::vector<AfsInfo::Entry>& AfsInfo::getEntries()
	{
		return entries;
	}

	void AfsInfo::build(const std::string& path)
	{
		entries.clear();

		entries.push_back({ "afsinfo.dat", 0 });
		for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
		{
			if (std::filesystem::is_directory(entry.path())) continue;
			std::cout << entry.path().string().substr(path.size() + 1) << " : " << std::filesystem::file_size(entry.path()) << std::endl;
			uint32_t filesize = static_cast<uint32_t>( std::filesystem::file_size( entry.path() ) );
			entries.push_back({ entry.path().string().substr(path.size() + 1), filesize });
		}
	}

	size_t AfsInfo::calculateSize()
	{
		size_t ret{ (entries.size() * 4) + 1 };
		for (auto& entry : entries)
			ret += entry.filename.size() + 2;
		return ret;
	}

	ByteArray AfsInfo::compile()
	{
		ByteArray ret;
		ret.resize(calculateSize());

		omemstream stream(ret.data(), ret.size());

		writeStrings(stream);
		writeSizes(stream);

		return ret;
	}

	void AfsInfo::writeStrings(std::ostream& stream)
	{
		for (auto& entry : entries)
			stream << entry.filename << "\r\n";
	}

	void AfsInfo::writeSizes(std::ostream& stream)
	{
		for (auto& entry : entries)
			stream << entry.size;
	}
}