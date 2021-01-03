#pragma once

#include "src/core/entities/Entity.h"

namespace cm
{
	class NavMesh
	{
	public:

		struct IndexedVertex
		{
			int32 indices[10];
			int32 index_count = 0;
			int32 triangle_index = -1;
			int32 this_index = -1;
			Vec3f position;

			inline void AddIndex(const int32 index)
			{
				ASSERT(index_count <= 10, "NavMesh::IndexedVertex to many vertex indices");
				for (int32 i = 0; i < index_count; i++)
				{
					if (index == indices[i])
					{
						return;
					}
				}

				indices[index_count] = index;
				index_count++;
			}
		};

		struct IndexedTriangle
		{
			int32 vertex1_index = -1;
			int32 vertex2_index = -1;
			int32 vertex3_index = -1;

			inline bool Equal(const IndexedTriangle other)
			{
				return (this->vertex1_index == other.vertex1_index) &&
					(this->vertex2_index == other.vertex2_index) &&
					(this->vertex3_index == other.vertex3_index);
			}

			inline bool HasVertex(const IndexedVertex other)
			{
				return (this->vertex1_index == other.this_index)
					|| (this->vertex2_index == other.this_index)
					|| (this->vertex3_index == other.this_index);
			}

			//inline int32 GetHash() // @TODO: Should we chache this ?
			//{
			//	return (vertex1_index + vertex2_index) * (vertex1_index + vertex2_index + 1) + vertex2_index + vertex3_index * 7;
			//}

			//bool operator<(const IndexedTriangle &other) const
			//{
			//	return vertex1_index < other.vertex1_index;
			//}
		};

		std::vector<IndexedVertex> vertices;
		std::vector<IndexedTriangle> triangles;

	public:
		void CreateNavMesh(const MeshCollider &world_collider);
		IndexedTriangle FindIndexedTriangleThatContainsPoint(const Vec3f &point) const;
		Triangle IndexedTriangleToTriangle(const IndexedTriangle &indexed_triangle) const;

	public:
		NavMesh();
		~NavMesh();

	private:
		int32 FindPositionIndex(const Vec3f &position);

	};

	class NavAgent : public Entity
	{
	public:
		NavMesh *nav_mesh; // @TODO: No raw pointer!!

		NavAgent();
		~NavAgent();

		std::vector<Vec3f> vertex_path;

		void FindPathTo(Vec3f point);

	private:
		struct TraversalNode
		{
			int32 parent;
			int32 index;

			TraversalNode() : parent(-1), index(-1) {}
			TraversalNode(const int32 &vertex_index, const int32 &parent) : parent(parent), index(vertex_index) {}
			~TraversalNode() {}
		};
	};
}
