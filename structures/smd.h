#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>

#include "interface/common.h"
#include "include/sys_io.h"
#include "tpl.h"
#include "bin.h"

using namespace Interface;

namespace smd
{
	template <std::endian endianness>
	struct Entry
	{
		Scalar<uint16_t, endianness> id{};
		le_uint8_t unknown_id{};
		le_uint8_t model_id{};
		ByteArray  unknown{ 0x3c, (std::byte)0 };		// prolly transform matrix

		Scalar<uint32_t, endianness> offset{};
		ByteArray rawdata;			// Replace with bin::BinFile

		void load(std::istream& stream)
		{
			stream >> id >> unknown_id >> model_id;
			stream.read((char*)unknown.data(), unknown.size());
		}
		void save(std::ostream& stream)
		{
			stream << id, model_id;
			stream.write((char*)unknown.data(), unknown.size());
		}
	};

	template <std::endian endianness>
	class SmdFile : protected Interface::Logger
	{
		const static inline uint32_t SMD_HEADER_GUARD { 0x40 };
		const static inline uint32_t MAX_COUNT		  { 2048 };

		size_t filesize{};

		le_uint16_t header_guard{ 0x0 };
		Scalar<uint16_t, endianness> model_count{};
		Scalar<uint16_t, endianness> entry_count{};
		Scalar<uint32_t, endianness> model_offset{};
		Scalar<uint32_t, endianness> texture_offset{};
		Scalar<uint32_t, endianness> padding{};

		tpl::TplFile Tpl;

		std::vector<Entry<endianness>> entries{};

	public:
		SmdFile(const Interface::Log& log) : Logger(&log), Tpl(log) {}
		SmdFile(const std::string& path, const Interface::Log& log) : Logger(&log), Tpl(log)
		{
			loadFromFile(path);
		}

		bool loadFromFile(const std::string& path)
		{
			print(Level::VERBOSE, "\tCreating File Stream.");
			std::ifstream stream(path, std::ios::ate + std::ios::binary);
			if (!stream.is_open()) return false;

			filesize = stream.tellg();
			print(Level::VERBOSE, "\tFile Size: " + std::to_string(filesize));
			stream.seekg(0);

			return loadFromStream(stream);
		}

		bool loadFromMemory(const ByteArray& data)
		{
			print(Level::VERBOSE, "\tCreating Memory Stream.\n");
			imemstream stream(data.data(), data.size());

			filesize = data.size();
			print(Level::VERBOSE, "\tFile Size: " + std::to_string(filesize));

			return loadFromStream(stream);
		}

		bool loadFromStream(std::istream& stream)
		{
			print(Level::LOG, "\tReading Header.");
			if (!readHeader(stream))
			{
				print(Level::EXCEPTION, EXCEPT_TEXT("Unable To Open SMD. File Failed Header Check"));
				return false;
			}
			print(Level::LOG, "\tReading Entries.");
			readEntryHeaders(stream);
			print(Level::LOG, "\tReading Offsets.");
			readOffsets(stream);
			print(Level::LOG, "\tFinding Highest Index.");
			findHighestIndex();
			print(Level::LOG, "\tReading BIN Files.");
			readBin(stream);
			print(Level::LOG, "\tReading TPL File.");
			readTpl(stream);

			return true;
		}

		bool readHeader(std::istream& stream)
		{
			print(Level::VERBOSE, "\t\tValidating Header...");
			stream >> header_guard;
			if (header_guard != SMD_HEADER_GUARD) return false;
			print(Level::VERBOSE, "\t\t\tSuccess!\n\t\tLoading Into Memory");
			stream >> entry_count >> model_offset >> texture_offset >> padding;
			print(Level::VERBOSE, "\t\t\tEntry Count: " + std::to_string(entry_count));
			print(Level::VERBOSE, "\t\t\tModels Offset: " + std::to_string(model_offset));
			print(Level::VERBOSE, "\t\t\tTPL Offset: " + std::to_string(texture_offset));
			if (entry_count > MAX_COUNT)
			{
				print(Level::ERROR, "Unable To Open Requested File: Failed Header Check");
				return false;
			}
			stream.seekg(0x40);														// Look into this. I think this is a global transform for all children
			return true;
		}

		void findHighestIndex()
		{
			for (auto& entry : entries)
				if (model_count < entry.id) model_count = entry.id;
			print(Level::VERBOSE, "\t\tHighest Index: " + std::to_string(model_count));
		}

		void readEntryHeaders(std::istream& stream)
		{
			print(Level::VERBOSE, "\t\tAllocating Memory...\n\t\tInitializing Entries..");
			entries.resize(entry_count.cast());
			for (auto& entry : entries)
			{
				entry.load(stream);
				print(Level::VERBOSE, "\t\t\tLoaded ID: " + std::to_string(entry.id));
			}
		}

		void readOffsets(std::istream& stream)
		{
			print(Level::VERBOSE, "\t\tReading Model Offsets..");
			stream.seekg(model_offset.cast());
			for (auto& entry : entries)
			{
				stream >> entry.offset;
				print(Level::VERBOSE, "\t\t\t" + std::to_string(entry.offset));
			}
		}

		void readBin(std::istream& stream)
		{
			print(Level::VERBOSE, "\t\tReading Models..");
			for (int i = 0; i < model_count; i++)
			{
				if (i == model_count - 1)
					entries[i].rawdata.resize(texture_offset - entries[i].offset);
				else
					entries[i].rawdata.resize(entries[i + 1].offset - entries[i].offset);

				print(Level::VERBOSE, "\t\t\t[" + std::to_string(entries[i].rawdata.size()) + "bytes] @ " +
					std::to_string(entries[i].offset.cast() + model_offset));
				stream.seekg(size_t(entries[i].offset.cast()) + model_offset);
				stream.read((char*)entries[i].rawdata.data(), entries[i].rawdata.size());
			}
		}

		void readTpl(std::istream& stream)
		{
			print(Level::VERBOSE, "\t\tReading TPL..");
			stream.seekg(texture_offset.cast() + 0x10);

			print(Level::VERBOSE, "\t\t\tGathering Rawdata..");
			ByteArray rawTplData{ filesize - (texture_offset - 0x10), (std::byte)0 };
			stream.read((char*)rawTplData.data(), rawTplData.size());
			print(Level::VERBOSE, "\t\t\tLoading TPL From Memory..");
			Tpl.loadFromMemory(rawTplData);
		}


		void extract(const size_t& index, const std::string& path)
		{
			if (index > model_count) return;

			print(Level::VERBOSE, "\t\tExtracting Index " + std::to_string(index) + " To: " + path);
			print(Level::VERBOSE, "\t\t\tCreating Filestream");
			std::ofstream file(path, std::ios::binary);
			if (!file.is_open()) return;
			print(Level::VERBOSE, "\t\t\t\tSuccess!");

			print(Level::VERBOSE, "\t\t\tGetting Entry");
			auto& current = entries[index];
			print(Level::VERBOSE, "\t\t\tWriting Entry To Disk");
			file.write((char*)current.rawdata.data(), current.rawdata.size());

			file.close();
		}

		void extract(const std::string& path)
		{
			print(Level::VERBOSE, "\t\tExtracting All Indices To: " + path);
			print(Level::VERBOSE, "\t\t\tCreating File Path");
			if (!std::filesystem::exists(path))
				std::filesystem::create_directory(path);

			print(Level::VERBOSE, "\t\tSaving Entries...");
			for (size_t i = 0; i < model_count; i++)
				extract(i, path + "/" + std::to_string(i) + ".bin");
		}


		void save(const std::string& path)
		{

		}


		const size_t& size() { return entries.size(); }

		const tpl::TplFile& getTpl()
		{
			return Tpl;
		}

		void saveTpl(const std::string& path)
		{
			Tpl.save(path);
		}

		void extractTpl(const std::string& path)
		{
			if (!std::filesystem::exists(path))
				std::filesystem::create_directory(path);
			Tpl.extractAll(path + "/");
		}
	};


}

