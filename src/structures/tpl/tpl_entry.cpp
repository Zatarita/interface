#include "structures/tpl.h"

namespace tpl
{
	void Entry::load(std::istream& stream)
	{
		loadTexture(stream);
	}

	void Entry::loadTexture(std::istream& stream)
	{
		texture.load(stream);
		if (texture.hasMipmaps()) 
			loadMipmaps(stream);
	}

	void Entry::loadMipmaps(std::istream& stream)
	{
		uint32_t start_pos = stream.tellg();

		for (int i = 0; i < texture.getMipmapCount(); i++)
		{
			auto paletteOffset = texture.getPaletteOffset();
			stream.seekg(texture.getMipmapOffset(i));
			mipmaps[i].load(stream, paletteOffset);
		}

		stream.seekg(start_pos);
	}

	void Entry::saveTexture(std::ostream& stream, bool include_mips)
	{
		writeTextureHeader(stream, include_mips);
		writeTextureData(stream, include_mips);
	}

	void Entry::writeTextureHeader(std::ostream& stream, bool include_mips)
	{
		texture.writeHeader(stream);

		auto start_pos = stream.tellp();
		if(include_mips)
			if (texture.hasMipmaps())
				for (int i = 0; i < texture.getMipmapCount(); i++) { stream.seekp(texture.getMipmapOffset(i)); mipmaps[i].writeHeader(stream, include_mips); }

		stream.seekp(start_pos);
	}

	void Entry::writeTextureData(std::ostream& stream, bool include_mips)
	{
		uint32_t start_pos = stream.tellp();

		stream.seekp(texture.getIndexOffset());
		texture.writeData(stream);
		if (include_mips)
			if (texture.hasMipmaps())
				for (int i = 0; i < texture.getMipmapCount(); i++) { stream.seekp(mipmaps[i].getIndexOffset()); mipmaps[i].writeData(stream, true); }

		stream.seekp(start_pos);
	}


	Texture& Entry::getTexture()
	{
		return texture;
	}

	Texture& Entry::getMipmap(const size_t& index)
	{
		if (index < 2 && index >= 0) return mipmaps[index];
		return mipmaps[0];
	}

	const uint16_t& Entry::getMipmapCount()
	{
		return texture.getMipmapCount();
	}

	bool Entry::hasMipmaps()
	{ 
		return texture.hasMipmaps(); 
	}
}
