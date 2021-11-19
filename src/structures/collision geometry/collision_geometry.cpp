#include <structures/collision_geometry.h>

using namespace Interface;

namespace CollisionGeometry
{
	void Collision::loadFromFile(const std::string& path)
	{
		std::ifstream stream(path, std::ios::binary);
		if (!stream.is_open())
			print(Level::EXCEPTION, EXCEPT_TEXT("Unable To Find Requested File!"));

		parseFile(stream);
	}

	void Collision::loadFromMemory(const ByteArray& data)
	{
		imemstream stream(data.data(), data.size());

		parseFile(stream);
	}

	void Collision::parseFile(std::istream& stream)
	{
		switch (validateHeader(stream))
		{
		case Type::Container:
			loadLayerOffsets(stream);
			break;
		case Type::Layer:
			allocateMemory(1);
			break;
		case Type::Unknown:
			print(Level::EXCEPTION, EXCEPT_TEXT("Unable To Validate Header!"));
			break;
		}

		loadLayers(stream);
	}

	Type Collision::validateHeader(std::istream& stream)
	{
		le_uint8_t header_guard;
		stream >> header_guard;
		if (header_guard.cast() == COLLISION_HEADER)
			return Type::Container;
		if (header_guard.cast() == LAYER_HEADER)
			return Type::Layer;
		return Type::Unknown;
			
	}

	void Collision::allocateMemory(const uint8_t& layerCount )
	{
		layer_offsets.resize(layerCount);
		layers.resize(layerCount, CollisionLayer(getLog()));
	}

	void Collision::loadLayerOffsets(std::istream& stream)
	{
		le_uint8_t layerCount;
		stream >> layerCount >> unknown;

		allocateMemory(layerCount);
		for (auto& offset : layer_offsets)
			stream >> offset;
	}

	void Collision::loadLayers(std::istream& stream)
	{
		for (int i = 0; i < layers.size(); i++)
		{
			stream.seekg(layer_offsets[i].cast());
			layers[i].load(stream);
		}
	}
}
