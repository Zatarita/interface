#include "structures/tpl.h"

namespace tpl
{
	void Entry::load(std::istream& stream)
	{
		print(Level::LOG, "\t\tInitializing Texture Load");
		loadTexture(stream);
	}

	void Entry::loadFromFile(const std::string& path)
	{
		print(Level::LOG, "\t\tInitializing Texture Load");
		size_t index{ 0 };
		std::string last;
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			last = entry.path().stem().string();
			//ret[index] = entry.path().string();
		}
		//texture.loadTGA(path);
	}

	void Entry::loadTexture(std::istream& stream)
	{
		print(Level::LOG, "\t\tLoading From Stream.");
		texture.load(stream);
		if (texture.hasMipmaps())
			loadMipmaps(stream);
		
	}

	void Entry::loadMipmaps(std::istream& stream)
	{
		auto start_pos = stream.tellg();

		for (int i = 0; i < texture.getMipmapCount(); i++)
		{
			auto paletteOffset = texture.getPaletteOffset();
			print(Level::LOG, "\t\tLoading Mipmap: [" + std::to_string(i) + "] " + std::to_string(paletteOffset));
			stream.seekg(texture.getMipmapOffset(i));
			mipmaps[i].load(stream, paletteOffset);
		}

		print(Level::VERBOSE, "\t\tSeeking To Starting Offset: " + std::to_string(start_pos));
		stream.seekg(start_pos);
	}

	void Entry::saveTexture(std::ostream& stream, bool include_mips)
	{
		print(Level::LOG, "\t\tWriting Container Header.");
		writeTextureHeader(stream, include_mips);
		print(Level::LOG, "\t\tWriting Container Data.");
		writeTextureData(stream, include_mips);
	}

	void Entry::writeTextureHeader(std::ostream& stream, bool include_mips)
	{
		texture.writeHeader(stream);

		auto start_pos = stream.tellp();
		if (include_mips)
		{
			if (texture.hasMipmaps())
			{
				print(Level::LOG, "\t\t\t\tWriting Mipmap Header.");
				for (int i = 0; i < texture.getMipmapCount(); i++) { stream.seekp(texture.getMipmapOffset(i)); mipmaps[i].writeHeader(stream, include_mips); }
			}
		}

		print(Level::VERBOSE, "\t\t\tSeeking To Starting Offset: " + std::to_string(start_pos));
		stream.seekp(start_pos);
	}

	void Entry::writeTextureData(std::ostream& stream, bool include_mips)
	{
		auto start_pos = stream.tellp();

		print(Level::VERBOSE, "\t\t\tSeeking Stream To Index Offset: " + std::to_string(texture.getIndexOffset()));
		stream.seekp(texture.getIndexOffset());
		print(Level::LOG, "\t\t\tWriting Texture Data To Stream.");
		texture.writeData(stream);

		if (include_mips)
			if (texture.hasMipmaps())
			{
				print(Level::LOG, "\t\t\t\tWriting Mipmap Data.");
				for (int i = 0; i < texture.getMipmapCount(); i++) { stream.seekp(mipmaps[i].getIndexOffset()); mipmaps[i].writeData(stream, true); }
			}

		print(Level::VERBOSE, "\t\t\tSeeking To Starting Offset: " + std::to_string(start_pos));
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

	uint16_t Entry::getMipmapCount()
	{
		return texture.getMipmapCount();
	}

	bool Entry::hasMipmaps()
	{ 
		return texture.hasMipmaps(); 
	}
}
