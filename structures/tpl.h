#pragma once

#include "include/sys_io.h"
#include "generic.h"
#include "interface/log.h"

#include <vector>
#include <fstream>
#include <string>
#include <filesystem>

using namespace debug;

namespace tpl
{
	static const uint8_t  TPL_HEADER_SIZE		  {0x10};
	static const uint8_t  TPL_ENTRY_SIZE		  {0x30};
	static const uint8_t  COLOR_PALETTE_SIZE_4BIT {0x80};
	static const uint16_t COLOR_PALETTE_SIZE_8BIT {0x400};
	static inline le_uint32_t ZERO{ 0x0 };

	class Texture					// Defined in src/structures/tpl/tpl_entry.cpp
	{
		struct InterlaceMode
		{
			enum : int
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
			le_uint16_t width;
			le_uint16_t height;
			le_uint16_t bit_depth;
			le_uint16_t interlacing;
			le_uint16_t unknown;
			le_uint16_t mipmap_count;
			le_uint32_t upscaling;

			mutable le_uint32_t mipmap_offset[2];
			le_uint32_t unknown2[2];

			mutable le_uint32_t index_offset;
			mutable le_uint32_t palette_offset;

			le_uint64_t unknown3;
		} header;

		std::vector<unsigned char> indices;
		std::vector<Color>		   palette;

		void allocateMemory();

		void loadIndices(std::istream& stream);
		void load4BitIndex(std::istream& stream);
		void load8BitIndex(std::istream& stream);

		void writeIndices(std::ostream& stream);
		void loadRLEIndices(std::istream& stream);
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

	public:
		void load(std::istream& stream, const uint32_t& palette_offset = 0);

		void writeHeader(std::ostream& stream, bool mipmap = false);
		void writeData(std::ostream& stream, bool mipmap = false);

		bool hasMipmaps() ;
		const uint16_t& getMipmapCount();
		const uint32_t& getMipmapOffset(const size_t i);
		const uint32_t& getPaletteOffset();
		const uint32_t& getIndexOffset();
		const uint32_t& getIndexSize();
		const uint32_t& getPaletteSize();

		void setIndexOffset(const uint32_t& newOffset)   const;
		void setPaletteOffset(const uint32_t& newOffset) const;
		void setMipmapOffset(const size_t& index, const uint32_t& newOffset) const;

		bool hasPalette();
	};

	class Entry
	{
		Texture texture, mipmaps[2];

		void loadTexture(std::istream& stream);
		void loadMipmaps(std::istream& stream);

		void writeTextureHeader(std::ostream& stream, bool include_mips = true);
		void writeTextureData(std::ostream& stream, bool include_mips = true);
	public:
		void load(std::istream& stream);
		void saveTexture(std::ostream& stream, bool include_mips = true);

		Texture& getTexture();
		Texture& getMipmap(const size_t& index);

		bool hasMipmaps();
		const uint16_t& getMipmapCount();
	};

	class TplFile
	{
		static inline const uint32_t HEADER_GUARD{ 0x1000 };
		static inline const uint32_t ENTRY_COUNT_GUARD{ 0x1000 };

		std::vector<tpl::Entry> entries;

		bool validateFile(std::istream& stream);
		uint32_t getEntryCount(std::istream& stream);
		void loadEntries(std::istream& stream);
	public:
		TplFile() = default;
		TplFile(const std::string & path) { load(path); };
		TplFile(const std::vector<tpl::Entry>& _entries) : entries(_entries) {};

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
	};

}