#pragma once

#include "include/sys_io.h"
#include "interface/common.h"
#include "afsinfo.h"

#include <string>
#include <fstream>
#include <vector>
#include <filesystem>

using namespace Interface;

namespace afs
{
	class AfsFile : protected Interface::Logger
	{
		struct Entry
		{
			le_uint32_t offset{}, size{};
			ByteArray rawData{};
		};

		struct FileTable
		{
			static inline const uint8_t FILENAME_LENGTH{ 0x20 };

			struct Entry
			{
				struct
				{
					le_uint16_t year{}, month{},  day{},
								hour{}, minute{}, second{};
				} time;

				std::string filename{ 0x20, '\0' };
				le_uint32_t filesize{};
			};

			le_uint32_t offset{}, size{};
			std::vector<Entry> entries{};

			void read(std::istream& stream);
			void write(std::ostream& stream);
		} file_table;

		static inline const uint16_t    BLOCK_ALLIGNMENT{ 0x800 };
		static inline const uint16_t    ENTRY_SIZE		{ 0x30 };
		static inline const std::string SIGNATURE		{ "AFS" };
		static inline const std::string SIZE0			{ "size0.dat" + std::string(0x27, '\0')};

		std::string signature{ "AFS\0" };
		le_uint32_t file_count{};
		std::vector<Entry> files{};

		dat::AfsInfo datInfo;

		size_t indexFromName(const std::string& name);

		bool validateHeader(std::istream& stream);
		void getFileCount(std::istream& stream);
		void allocateHeaderMemory();
		void readEntryHeaders(std::istream& stream);
		void readFileTableHeader(std::istream& stream);
		void allocateFileMemory();
		void readEntries(std::istream& stream);
		void readFileTable(std::istream& stream);

		void readDatInfo();

		void buildAfsInfo(const std::string path);

		void buildFileTable();
		void calculateOffsets();
		void writeFile(const std::string& path);
		void writeHeader(std::ostream& stream);
		void readFilesToEntries(const std::string& path);
		void writeEntryHeaders(std::ostream& stream);
		void writeEntries(std::ostream& stream);
		void writeFileTable(std::ostream& stream);
		void padRemainingFile(std::ostream& stream);
		void generateAfsInfo();
	public:
		AfsFile(const Interface::Log& log) : Logger(&log), datInfo(log) {};
		AfsFile(const std::string& path, const Interface::Log& log);

		bool loadFromFile(const std::string& path);

		void extract(const size_t& index, const std::string& path, const bool& ignore_empty);
		void extract(const std::string& name, const std::string& path, const bool& ignore_empty);
		void extractAll(const std::string& path, const bool& ignore_empty = false);

		void buildAfs(const std::string& path, const std::string& out);
		void save(const std::string& path);

		std::vector<std::string> getFileNames();
		std::vector<std::string> getFilePaths();
	};
}
