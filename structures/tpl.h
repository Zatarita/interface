#pragma once

#include "include/sys_io.h"
#include "interface/common.h"
#include "interface/log.h"
#include "tga.h"

#include <vector>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdio>

using namespace Interface;

namespace tpl
{
	static const uint8_t  TPL_HEADER_SIZE		  {0x10};
	static const uint8_t  TPL_ENTRY_SIZE		  {0x30};
	static const uint8_t  COLOR_PALETTE_SIZE_4BIT {0x80};
	static const uint16_t COLOR_PALETTE_SIZE_8BIT {0x400};
	static inline le_uint32_t ZERO{ 0x0 };

	class Texture : protected Interface::Logger					// Defined in src/structures/tpl/tpl_entry.cpp
	{
		struct InterlaceMode
		{
			enum : short
			{
				NONE,
				RGB,
				RGBp1,
				PS2
			};
		};

		struct Bitdepth
		{
			enum : int
			{
				COLOR_RLE = 6,
				COLOR_4BIT = 8,
				COLOR_8BIT
			};
		};
		

		struct Header
		{
			le_uint16_t width{};
			le_uint16_t height{};
			le_uint16_t bit_depth{};
			le_uint16_t interlacing{};
			le_uint16_t unknown{};
			le_uint16_t mipmap_count{};
			le_uint32_t upscaling{};

			mutable le_uint32_t mipmap_offset[2]{};
			le_uint32_t unknown2[2]{};

			mutable le_uint32_t index_offset{};
			mutable le_uint32_t palette_offset{};

			le_uint64_t unknown3{};
		} header{};

		std::vector<unsigned char>	  indices{};
		std::vector<Interface::Color> palette{};

		void allocateMemory();

		void loadIndices(std::istream& stream);
		void load4BitIndex(std::istream& stream);
		void load8BitIndex(std::istream& stream);

		void writeIndices(std::ostream& stream);
		void write4BitIndex(std::ostream& stream);
		void write8BitIndex(std::ostream& stream);

		void loadPalette(std::istream& stream);
		void loadRLEPalette(std::istream& stream);
		void load4BitPalete(std::istream& stream);
		void load8itPalete(std::istream& stream);

		void writePalette(std::ostream& stream);
		void writeRLEPalette(std::ostream& stream);
		void write4BitPalette(std::ostream& stream);
		void write8BitPalette(std::ostream& stream);

		std::vector<unsigned char> unswizzle(const std::vector<unsigned char>&);

	public:
		Texture(const Interface::Log& log) : Logger(&log) {}
		void load(std::istream& stream, const uint32_t& palette_offset = 0);

		void writeHeader(std::ostream& stream, bool mipmap = false);
		void writeData(std::ostream& stream, bool mipmap = false);

		bool hasMipmaps() ;
		uint16_t getMipmapCount();
		uint32_t getMipmapOffset(const size_t i);
		uint32_t getPaletteOffset();
		uint32_t getIndexOffset();
		uint32_t getIndexSize();
		uint32_t getPaletteSize();

		void setIndexOffset(const uint32_t& newOffset)   const;
		void setPaletteOffset(const uint32_t& newOffset) const;
		void setMipmapOffset(const size_t& index, const uint32_t& newOffset) const;

		bool hasPalette();
		void saveTGA(const std::string& path);
		//void loadTGA(const std::string& path);
	};

	class Entry : protected Interface::Logger
	{
		Texture texture, mipmaps[2];

		void loadTexture(std::istream& stream);
		void loadMipmaps(std::istream& stream);

		void writeTextureHeader(std::ostream& stream, bool include_mips = true);
		void writeTextureData(std::ostream& stream, bool include_mips = true);
	public:
		Entry(const Interface::Log& log) : Logger(&log), texture(log), mipmaps{ {log}, {log} } {}

		void load(std::istream& stream);
		void loadFromFile(const std::string& path);
		void loadMipmapsFromFile(const std::string& path);
		void saveTexture(std::ostream& stream, bool include_mips = true);

		Texture& getTexture();
		Texture& getMipmap(const size_t& index);

		bool hasMipmaps();
		uint16_t getMipmapCount();
	};

	class TplFile : protected Interface::Logger
	{
		static inline const uint32_t HEADER_GUARD{ 0x1000 };
		static inline const uint32_t ENTRY_COUNT_GUARD{ 0x1000 };

		std::vector<tpl::Entry> entries;

		bool validateFile(std::istream& stream);
		uint32_t getEntryCount(std::istream& stream);
		void loadEntries(std::istream& stream);
	public:
		TplFile(const Interface::Log& log) : Logger(&log) {}
		TplFile(const std::string & path, const Interface::Log& log) : Logger(&log) { load(path); };
		TplFile(const std::vector<tpl::Entry>& _entries, const Interface::Log& log) : Logger(&log), entries(_entries) {};

		void load(const std::string& path);
		void loadFromMemory(const ByteArray& data);

		void save(const std::string& path, bool include_mips = true);
		void calculateOffsets(bool include_mips = true);
		void writeHeader(std::ostream& stream);
		void writeEntries(std::ostream& stream, bool include_mips = true);

		TplFile& operator += (TplFile& rhs);
		std::vector<tpl::Entry>& getEntries() { return entries; }

		void extractAll(const std::string& path, bool include_mips = true);
		void extract(const size_t index, const std::string& path, bool include_mips = true);

		void decompileAll(const std::string& path, bool include_mips = true);
		void decompile(const size_t index, const std::string& path, bool include_mips = true);
		void compile(const std::string& path);
	};

}