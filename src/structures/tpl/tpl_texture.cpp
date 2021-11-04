#include "structures/tpl.h"

namespace tpl
{
	void Texture::load(std::istream& stream, const uint32_t& palette_offset)
	{
		stream >> header.width >> header.height >> header.bit_depth >> header.interlacing
			>> header.unknown >> header.mipmap_count >> header.upscaling
			>> header.mipmap_offset[0] >> header.mipmap_offset[1]
			>> header.unknown2[0] >> header.unknown2[1]
			>> header.index_offset >> header.palette_offset >> header.unknown3;

		if (palette_offset) header.palette_offset = palette_offset;

		allocateMemory();
		loadIndices(stream);
		loadPalette(stream);
	}

	void Texture::setIndexOffset  (const uint32_t& newOffset) const { header.index_offset   = newOffset; }
	void Texture::setPaletteOffset(const uint32_t& newOffset) const { header.palette_offset = newOffset; }
	void Texture::setMipmapOffset(const size_t& index, const uint32_t& newOffset) const
	{
		if (index < 2 && index >= 0)
			header.mipmap_offset[index] = newOffset;
	}

	bool Texture::hasMipmaps()							     { return header.mipmap_offset[0] | header.mipmap_offset[1]; }
	const uint16_t& Texture::getMipmapCount()				 { return hasMipmaps() ? 2 : 0; }	// change this at some point
	const uint32_t& Texture::getMipmapOffset(const size_t i) { return header.mipmap_offset[i].cast(); }
	const uint32_t& Texture::getPaletteOffset()
	{ 
		return header.palette_offset; 
	}
	const uint32_t& Texture::getIndexOffset()				   { return header.index_offset.cast(); }

	const uint32_t& Texture::getIndexSize() 
	{
		switch (header.bit_depth.cast())
		{
		case Bitdepth::COLOR_RLE:
			return palette.size() * 4;
			break;
		case Bitdepth::COLOR_4BIT:
			return indices.size() / 2;
			break;
		case Bitdepth::COLOR_8BIT:
			return indices.size();
			break;
		default:
			std::cout << "\tUnable To Load TPL Indices: Unknown Bitdepth [" + std::to_string(header.bit_depth.cast()) + "]\n";
		}
		return 0;
	}

	const uint32_t& Texture::getPaletteSize()
	{
		switch (header.bit_depth.cast())
		{
		case Bitdepth::COLOR_RLE:
			return indices.size() * 4;
			break;
		case Bitdepth::COLOR_4BIT:
			return COLOR_PALETTE_SIZE_4BIT;
			break;
		case Bitdepth::COLOR_8BIT:
			return COLOR_PALETTE_SIZE_8BIT;
			break;
		}
	}


	void Texture::writeHeader(std::ostream& stream, bool mipmap)
	{
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
		writeIndices(stream);
		if(!mipmap)
			writePalette(stream);
	}

	void Texture::allocateMemory()
	{
		if (hasPalette())
		{
			indices.resize(size_t(header.width.cast()) * header.height.cast());
			if (palette.size() != 0x100) palette.resize(0x100);
		}
		else
			palette.resize(size_t(header.width.cast()) * header.height.cast());
	}

	void Texture::loadIndices(std::istream& stream)
	{
		auto start_pos = stream.tellg();
		stream.seekg(header.index_offset.cast());

		switch (header.bit_depth.cast())
		{
		case Bitdepth::COLOR_RLE:
			break;
		case Bitdepth::COLOR_4BIT:
			load4BitIndex(stream);
			break;
		case Bitdepth::COLOR_8BIT:
			load8BitIndex(stream);
			break;
		default:
			std::cout << "\tUnable To Load TPL Indices: Unknown Bitdepth [" + std::to_string(header.bit_depth.cast()) + "]\n";
		}

		stream.seekg(start_pos);
	}

	void Texture::loadRLEIndices(std::istream& stream)
	{
		for (size_t i = 0; i < indices.size(); i++)
			indices[i] = i;
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
			stream.seekg(header.palette_offset.cast());
		else
			stream.seekg(header.index_offset.cast());

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
			std::cout << "\tUnable To Load TPL Color Palette: Unknown Bitdepth [" + std::to_string(header.bit_depth.cast()) + "]\n";
		}

		stream.seekg(start_pos);
	}

	void Texture::loadRLEPalette(std::istream& stream)
	{
		for (auto& color : palette)
			stream >> color;
	}


	void Texture::load4BitPalete(std::istream& stream)
	{
		for (int i = 0; i < 8; i++)
			stream >> palette[i];
		stream.ignore(0x20);
		for (int i = 8; i < 16; i++)
			stream >> palette[i];
		stream.ignore(0x20);
	}

	void Texture::load8itPalete(std::istream& stream)
	{
		for (auto& color : palette)
			stream >> color;
	}

	void Texture::writeIndices(std::ostream& stream)
	{
		auto start_pos = stream.tellp();
		stream.seekp(header.index_offset.cast());

		switch (header.bit_depth.cast())
		{
		case Bitdepth::COLOR_RLE:
			break;
		case Bitdepth::COLOR_4BIT:
			write4BitIndex(stream);
			break;
		case Bitdepth::COLOR_8BIT:
			write8BitIndex(stream);
			break;
		}

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
		if(hasPalette())
			stream.seekp(header.palette_offset.cast());
		else
			stream.seekp(header.index_offset.cast());

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
			std::cout << "Unable To Load TPL Color Palette: Unknown Bitdepth [" + std::to_string(header.bit_depth.cast()) + "]";
		}

		stream.seekp(start_pos);
	}

	void Texture::writeRLEPalette(std::ostream& stream)
	{
		for (auto& color : palette)
			stream << color;
	}

	void Texture::write4BitPalette(std::ostream& stream)
	{
		for (int i = 0; i < 8; i++)
			stream << palette[i];
		stream.seekp((int)stream.tellp() + 32);
		for (int i = 8; i < 16; i++)
			stream << palette[i];
		stream.seekp((int)stream.tellp() + 28);
		stream << le_uint32_t(0);
	}

	void Texture::write8BitPalette(std::ostream& stream)
	{
		for (auto& color : palette)
			stream << color;
	}

	bool Texture::hasPalette()
	{
		return header.bit_depth != Bitdepth::COLOR_RLE;
	}
}
