#pragma once

#include <vector>
#include <fstream>
#include <string>

#include "sys_io.h"
#include "interface/common.h"

namespace CollisionGeometry
{
	template<class coordinate_t>
	struct Coordinate
	{
		coordinate_t x, y, z;
	};

	enum class FaceType
	{
		FLOOR,
		CEILING,
		WALL
	};

	enum class Type
	{
		Container,
		Layer,
		Unknown
	};

	struct Face
	{
		FaceType usage;

		Coordinate<le_uint16_t> indices{};
		le_uint16_t normal{};
		Coordinate<le_uint16_t> edges{};
		le_uint16_t unknown{};
		le_uint32_t face_meta{};					// TODO expand this to actually hold meta info after studying more
	};

	class FaceGroup // todo
	{
		Coordinate<float> BoundingFrom{}, BoundingTo{};

		le_uint16_t faces_floor;
		le_uint16_t faces_ceiling;
		le_uint16_t faces_walls;
	};

	class CollisionLayer : public Interface::Logger
	{
		static inline const uint8_t LAYER_HEADER{ 0x20 };

		le_uint16_t coordinate_count{};
		le_uint16_t normal_count{};
		le_uint16_t edge_count{};
		le_uint16_t unknown{};
		le_uint16_t total_faces{};
		le_uint16_t floor_faces{};
		le_uint16_t ceiling_faces{};
		le_uint16_t wall_faces{};
		le_uint16_t face_group_count{};
		le_uint16_t unknown2{};

		std::vector<Coordinate<le_float>> coordinates;
		std::vector<Coordinate<le_float>> normals;
		std::vector<Coordinate<le_float>> edges;
		std::vector<Face> faces;
		std::vector<FaceGroup> face_groups;


		void loadHeader(std::istream& stream);
		void loadCoordinates(std::istream& stream);
		void allocateMemory();
		void loadNormals(std::istream& stream);
		void loadEdges(std::istream& stream);
		void loadFaces(std::istream& stream);
	public:
		CollisionLayer(const Interface::Log& log) : Logger(&log) {}

		void load(std::istream& stream);

		bool validateHeader(std::istream& stream);
	};

	class Collision : public Interface::Logger
	{
		static inline const uint8_t COLLISION_HEADER{ 0x80 };
		static inline const uint8_t LAYER_HEADER{ 0x20 };

		le_uint8_t header_guard{};
		le_uint16_t unknown{};

		std::vector<le_uint32_t> layer_offsets{};
		std::vector<CollisionLayer> layers{};

		Type validateHeader(std::istream& stream);
		void loadLayerOffsets(std::istream& stream);
		void allocateMemory(const uint8_t&);
		void loadLayers(std::istream& stream);
		void parseFile(std::istream& stream);
	public:
		Collision(const Interface::Log& log) : Logger(&log) {}
		Collision(const std::string& path, const Interface::Logger& log) : Logger(log) { loadFromFile(path); }

		void loadFromFile(const std::string& path);
		void loadFromMemory(const ByteArray& data);
	};

	using SAT = Collision;
	using EAT = Collision;
}
