#include "structures/collision_geometry.h"

using namespace Interface;

namespace CollisionGeometry
{

	void CollisionLayer::load(std::istream& stream)
	{
		if (!validateHeader(stream))
			print(Level::EXCEPTION, EXCEPT_TEXT("Unable To Load Requested Layer: Invaliid Header Check."));
		
		loadHeader(stream);
		allocateMemory();
		loadCoordinates(stream);
		loadNormals(stream);
		loadEdges(stream);
		loadFaces(stream);
		//loadFaceGroups(stream);
	}

	void CollisionLayer::loadHeader(std::istream& stream)
	{
		stream >> coordinate_count >> normal_count >> edge_count >> unknown;
		stream >> total_faces >> floor_faces >> wall_faces >> face_group_count >> unknown2;
	}

	void CollisionLayer::allocateMemory()
	{
		coordinates.resize(coordinate_count);
		normals.resize(normal_count);
		edges.resize(edge_count);
		faces.resize(total_faces);
		face_groups.resize(coordinate_count);
	}

	void CollisionLayer::loadCoordinates(std::istream& stream)
	{
		for (auto& coordinate : coordinates)
			stream >> coordinate.x >> coordinate.y >> coordinate.z;
	}


	void CollisionLayer::loadNormals(std::istream& stream)
	{
		for (auto& coordinate : normals)
			stream >> coordinate.x >> coordinate.y >> coordinate.z;
	}


	void CollisionLayer::loadEdges(std::istream& stream)
	{
		for (auto& coordinate : edges)
			stream >> coordinate.x >> coordinate.y >> coordinate.z;
	}


	void CollisionLayer::loadFaces(std::istream& stream)
	{
		for (int i = 0; i < floor_faces; i++)
		{
			stream >> faces[i].indices.x >> faces[i].indices.y >> faces[i].indices.z
				   >> faces[i].normal
				   >> faces[i].edges.x >> faces[i].edges.y >> faces[i].edges.z >> faces[i].unknown >> faces[i].face_meta;
			faces[i].usage = FaceType::FLOOR;

			if (faces[i].indices.x > coordinate_count, faces[i].indices.y > coordinate_count, faces[i].indices.z > coordinate_count)
				print(Level::EXCEPTION, EXCEPT_TEXT("Model Index Exceeds Face Count"));
		}

		for (int i = floor_faces; i < floor_faces + ceiling_faces; i++)
		{
			stream >> faces[i].indices.x >> faces[i].indices.y >> faces[i].indices.z
				   >> faces[i].normal
				   >> faces[i].edges.x >> faces[i].edges.y >> faces[i].edges.z >> faces[i].unknown >> faces[i].face_meta;
			faces[i].usage = FaceType::CEILING;

			if (faces[i].indices.x > coordinate_count, faces[i].indices.y > coordinate_count, faces[i].indices.z > coordinate_count)
				print(Level::EXCEPTION, EXCEPT_TEXT("Model Index Exceeds Face Count"));
		}

		for (int i = floor_faces + ceiling_faces; i < total_faces; i++)
		{
			stream >> faces[i].indices.x >> faces[i].indices.y >> faces[i].indices.z
				   >> faces[i].normal
				   >> faces[i].edges.x >> faces[i].edges.y >> faces[i].edges.z >> faces[i].unknown >> faces[i].face_meta;
			faces[i].usage = FaceType::WALL;

			if (faces[i].indices.x > coordinate_count, faces[i].indices.y > coordinate_count, faces[i].indices.z > coordinate_count)
				print(Level::EXCEPTION, EXCEPT_TEXT("Model Index Exceeds Face Count"));
		}
	}

	bool CollisionLayer::validateHeader(std::istream& stream)
	{
		le_uint16_t header_guard;
		stream >> header_guard;
		if (header_guard.cast() != LAYER_HEADER)
			return false;
		return true;
	}
}
