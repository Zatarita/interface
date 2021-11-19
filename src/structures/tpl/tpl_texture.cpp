#include "structures/tpl.h"

namespace tpl
{
	void Texture::load(std::istream& stream, const uint32_t& palette_offset)
	{
		print(Level::LOG, "\t\t\tReading Header.");
		stream >> header.width >> header.height >> header.bit_depth >> header.interlacing
			>> header.unknown >> header.mipmap_count >> header.upscaling
			>> header.mipmap_offset[0] >> header.mipmap_offset[1]
			>> header.unknown2[0] >> header.unknown2[1]
			>> header.index_offset >> header.palette_offset >> header.unknown3;

		if (palette_offset) header.palette_offset = palette_offset;

		print(Level::LOG, "\t\t\tAllocating Memory.");
		allocateMemory();
		print(Level::LOG, "\t\t\tLoading Indices.");
		loadIndices(stream);
		print(Level::LOG, "\t\t\tLoading Palette.");
		loadPalette(stream);
	}

	void Texture::setIndexOffset  (const uint32_t& newOffset) const { header.index_offset   = newOffset; }
	void Texture::setPaletteOffset(const uint32_t& newOffset) const { header.palette_offset = newOffset; }
	void Texture::setMipmapOffset(const size_t& index, const uint32_t& newOffset) const
	{
		if (index < 2 && index >= 0)
			header.mipmap_offset[index] = newOffset;
	}

	bool Texture::hasMipmaps()									{ return header.mipmap_offset[0] | header.mipmap_offset[1]; }
	uint16_t Texture::getMipmapCount()							{ return hasMipmaps() ? 2 : 0; }	// change this at some point
	uint32_t Texture::getMipmapOffset(const size_t i)			{ return header.mipmap_offset[i].cast(); }
	uint32_t Texture::getPaletteOffset()
	{ 
		return header.palette_offset; 
	}
	uint32_t Texture::getIndexOffset()							{ return header.index_offset.cast(); }

	uint32_t Texture::getIndexSize() 
	{
		switch (header.bit_depth.cast())
		{
		case Bitdepth::COLOR_RLE:
			return static_cast<uint32_t>(palette.size()) * 4;
			break;
		case Bitdepth::COLOR_4BIT:
			return static_cast<uint32_t>(indices.size()) / 2;
			break;
		case Bitdepth::COLOR_8BIT:
			return static_cast<uint32_t>(indices.size());
			break;
		default:
			std::cout << "\tUnable To Load TPL Indices: Unknown Bitdepth [" + std::to_string(header.bit_depth.cast()) + "]\n";
		}
		return 0;
	}

	uint32_t Texture::getPaletteSize()
	{
		switch (header.bit_depth.cast())
		{
		case Bitdepth::COLOR_RLE:
			return static_cast<uint32_t>(indices.size()) * 4;
			break;
		case Bitdepth::COLOR_4BIT:
			return COLOR_PALETTE_SIZE_4BIT;
			break;
		case Bitdepth::COLOR_8BIT:
			return COLOR_PALETTE_SIZE_8BIT;
			break;
		}
		return 0;
	}


	void Texture::writeHeader(std::ostream& stream, bool mipmap)
	{
		print(Level::LOG, "\t\t\tWriting Header.");
		stream << header.width << header.height << header.bit_depth << header.interlacing
			<< header.unknown << header.mipmap_count << header.upscaling
			<< header.mipmap_offset[0] << header.mipmap_offset[1]
			<< header.unknown2[0] << header.unknown2[1]
			<< header.index_offset;
		if (mipmap)
			stream << ZERO;
		else
			stream << header.palette_offset;
		stream << header.unknown3;			// Palette offset in mipmaps = 0
	}

	void Texture::writeData(std::ostream& stream, bool mipmap)
	{
		print(Level::LOG, "\t\t\t\tWriting Indices.");
		writeIndices(stream);
		if (!mipmap)
		{
			print(Level::LOG, "\t\t\t\tWriting Palette.");
			writePalette(stream);
		}
			
	}

	void Texture::allocateMemory()
	{
		if (hasPalette())
		{
			indices.resize(size_t(header.width.cast()) * header.height.cast());
			print(Level::VERBOSE, "\t\t\t\tIndex Array Size: " + std::to_string(indices.size()));
			if (palette.size() != 0x100) palette.resize(0x100);
			print(Level::VERBOSE, "\t\t\t\tPalette Array Size: " + std::to_string(palette.size()));
		}
		else
		{
			palette.resize(size_t(header.width.cast()) * header.height.cast());
			print(Level::VERBOSE, "\t\t\t\tPalette: " + std::to_string(palette.size()));
		}
	}

	void Texture::loadIndices(std::istream& stream)
	{
		auto start_pos = stream.tellg();
		stream.seekg(header.index_offset.cast());
		print(Level::VERBOSE, "\t\t\t\tSeeking To Indices Offset: " + std::to_string(header.index_offset.cast()));

		switch (header.bit_depth.cast())
		{
		case Bitdepth::COLOR_RLE:
			print(Level::LOG, "\t\t\t\tRLE (No Indices To Load)");
			break;
		case Bitdepth::COLOR_4BIT:
			load4BitIndex(stream);
			break;
		case Bitdepth::COLOR_8BIT:
			load8BitIndex(stream);
			break;
		default:
			print(Level::ERROR, "\t\t\t\tUnable To Load TPL Indices: Unknown Bitdepth [" + std::to_string(header.bit_depth.cast()) + "]");
		}

		print(Level::VERBOSE, "\t\t\t\tSeeking To Starting Offset: " + std::to_string(start_pos));
		stream.seekg(start_pos);
	}

	void Texture::load4BitIndex(std::istream& stream)
	{
		for (size_t i = 0; i < indices.size();)
		{
			unsigned char buffer;
			stream.read((char*)&buffer, 1);

			indices[i++] = buffer & 0xF;
			indices[i++] = (buffer >> 4) & 0xF;
		}
	}

	void Texture::load8BitIndex(std::istream& stream)
	{
		stream.read((char*)indices.data(), indices.size());
	}

	void Texture::loadPalette(std::istream& stream)
	{
		auto start_pos = stream.tellg();

		if (hasPalette())
		{
			print(Level::VERBOSE, "\t\t\t\tSeeking To Palette Offset: " + std::to_string(header.index_offset.cast()));
			stream.seekg(header.palette_offset.cast());
		}
		else
		{
			print(Level::VERBOSE, "\t\t\t\tNon-Palette Format - Seeking To Index Offset Instead: " + std::to_string(header.index_offset.cast()));
			stream.seekg(header.index_offset.cast());
		}

		switch (header.bit_depth)
		{
		case Bitdepth::COLOR_RLE:
			loadRLEPalette(stream);
			break;
		case Bitdepth::COLOR_4BIT:
			load4BitPalete(stream);
			break;
		case Bitdepth::COLOR_8BIT:
			load8itPalete(stream);
			break;
		default:
			print(Level::ERROR, "\t\t\t\tUnable To Load TPL Color Palette: Unknown Bitdepth [" + std::to_string(header.bit_depth.cast()) + "]");
		}

		stream.seekg(start_pos);
	}

	void Texture::loadRLEPalette(std::istream& stream)
	{
		print(Level::VERBOSE, "\t\t\t\t\tLoading RLE Palette");
		for (auto& color : palette)
			stream >> color;
	}


	void Texture::load4BitPalete(std::istream& stream)
	{
		print(Level::VERBOSE, "\t\t\t\t\tLoading 4-bit Palette");
		for (int i = 0; i < 8; i++)
			stream >> palette[i];
		stream.ignore(0x20);
		for (int i = 8; i < 16; i++)
			stream >> palette[i];
		stream.ignore(0x20);
	}

	void Texture::load8itPalete(std::istream& stream)
	{
		print(Level::VERBOSE, "\t\t\t\t\tLoading 8-bit Palette");
		for (auto& color : palette)
			stream >> color;
	}

	void Texture::writeIndices(std::ostream& stream)
	{
		auto start_pos = stream.tellp();
		stream.seekp(header.index_offset.cast());
		print(Level::VERBOSE, "\t\t\t\t\tSeeking To Index Offset: " + std::to_string(header.index_offset));
		
		switch (header.bit_depth.cast())
		{
		case Bitdepth::COLOR_RLE:
			print(Level::LOG, "\t\t\t\t\tRLE No Indices To Write.");
			break;
		case Bitdepth::COLOR_4BIT:
			write4BitIndex(stream);
			break;
		case Bitdepth::COLOR_8BIT:
			write8BitIndex(stream);
			break;
		}

		print(Level::VERBOSE, "\t\t\t\t\tSeeking To Starting Offset: " + std::to_string(start_pos));
		stream.seekp(start_pos);
	}

	void Texture::write4BitIndex(std::ostream& stream)
	{
		char buffer;
		for (size_t i = 0; i < indices.size(); i += 2)
		{
			buffer = indices[i + 1] * 0x10 + (indices[i]);
			stream.write(&buffer, 1);
		}
	}

	void Texture::write8BitIndex(std::ostream& stream)
	{
		for (auto& index : indices)
			stream << index;
	}

	void Texture::writePalette(std::ostream& stream)
	{
		auto start_pos = stream.tellp();
		if (hasPalette())
		{
			print(Level::VERBOSE, "\t\t\t\t\tSeeking To Palette Offset: " + std::to_string(header.palette_offset.cast()));
			stream.seekp(header.palette_offset.cast());
		}
		else
		{
			print(Level::VERBOSE, "\t\t\t\t\tNon-Palette Format - Seeking To Index Offset Instead: " + std::to_string(header.index_offset.cast()));
			stream.seekp(header.index_offset.cast());
		}

		switch (header.bit_depth)
		{
		case Bitdepth::COLOR_RLE:
			writeRLEPalette(stream);
			break;
		case Bitdepth::COLOR_4BIT:
			write4BitPalette(stream);
			break;
		case Bitdepth::COLOR_8BIT:
			write8BitPalette(stream);
			break;
		default:
			print(Level::ERROR, "\t\t\t\t\tUnable To Load TPL Color Palette : Unknown Bitdepth[" + std::to_string(header.bit_depth.cast()) + "]");
		}

		stream.seekp(start_pos);
	}

	void Texture::writeRLEPalette(std::ostream& stream)
	{
		print(Level::LOG, "\t\t\t\t\tWriting RLE Palette");
		for (auto& color : palette)
			stream << color;
	}

	void Texture::write4BitPalette(std::ostream& stream)
	{
		print(Level::LOG, "\t\t\t\t\tWriting 4-bit Palette");
		for (int i = 0; i < 8; i++)
			stream << palette[i];
		stream.seekp((size_t)stream.tellp() + 32);
		for (int i = 8; i < 16; i++)
			stream << palette[i];
		stream.seekp((size_t)stream.tellp() + 28);
		stream << le_uint32_t(0);
	}

	void Texture::write8BitPalette(std::ostream& stream)
	{
		print(Level::LOG, "\t\t\t\t\tWriting 8-bit Palette");
		for (auto& color : palette)
			stream << color;
	}

	bool Texture::hasPalette()
	{
		return header.bit_depth != Bitdepth::COLOR_RLE;
	}

	void Texture::saveTGA(const std::string& path)
	{
		tga::Header newHeader{ 0, 0, 2, 0, 0, 0, 0, 0, header.width, header.height, 0x20, 0x00 };
		
		std::ofstream test(path, std::ios::binary);
		if (!test.is_open()) return;

		test << newHeader;

		std::vector<unsigned char> pixels;
		if (header.interlacing.cast() == InterlaceMode::PS2)
			pixels = indices;
		else
			pixels = indices;


		if (hasPalette())
			for (auto index : pixels)
				palette[index].writeNormalized(test);
		else
			for (auto color : palette)
				color.writeNormalized(test);
		test << le_uint8_t(1);
	}

	std::vector<unsigned char> Texture::unswizzle(const std::vector<unsigned char>& originalPixels)
	{
		static unsigned char InterlaceMatrix[] = {
			0x00, 0x10, 0x02, 0x12,
			0x11, 0x01, 0x13, 0x03,
		};

		static int Matrix[]        = { 0, 1, -1, 0 };
		static int TileMatrix[]    = { 4, -4 };

		std::vector<unsigned char> newPixels( size_t(header.width) * size_t(header.height), '\0' );
		for (int y = 0; y < header.height; y++)
		{
			bool oddRow = ((y & 1) != 0);

			int num1   = ((y / 4) & 1);
			int num3   = (y % 4);

			for (int x = 0; x < header.width; x++)
			{
				int num2 = ((x / 4) & 1);

				int num4 = ((x / 4) % 4);
				if (oddRow) num4 += 4;

				int num5 = ((x * 4) % 16);
				int num6 = ((x / 16) * 32);

				int num7 = (oddRow) ? ((y - 1) * header.width) : (y * header.width);

				int xx = x + num1 * TileMatrix[num2];
				int yy = y + Matrix[num3];

				int i = InterlaceMatrix[num4] + num5 + num6 + num7;
				int j = yy * header.width + xx;

				newPixels[j] = originalPixels[i];
			}
		}

		return newPixels;
	}
}