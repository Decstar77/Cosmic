#include "NavMesh.h"
#include <set>

namespace cm
{
	void NavMesh::CreateNavMesh(const MeshCollider &world_collider)
	{
		triangles.reserve(world_collider.size());

		for (int32 i = 0; i < world_collider.size(); i++)
		{
			const Triangle &tri = world_collider.at(i);

			int32 triangle_index = static_cast<int32>(triangles.size());

			int32 vert1_index = -1;
			int32 vert2_index = -1;
			int32 vert3_index = -1;

			vert1_index = FindPositionIndex(tri.v0);
			if (vert1_index == -1)
			{
				vert1_index = static_cast<int32>(vertices.size());
				IndexedVertex &vert = vertices.emplace_back();
				vert.triangle_index = triangle_index;
				vert.position = tri.v0;
			}

			vert2_index = FindPositionIndex(tri.v1);
			if (vert2_index == -1)
			{
				vert2_index = static_cast<int32>(vertices.size());
				IndexedVertex &vert = vertices.emplace_back();
				vert.triangle_index = triangle_index;
				vert.position = tri.v1;
			}

			vert3_index = FindPositionIndex(tri.v2);
			if (vert3_index == -1)
			{
				vert3_index = static_cast<int32>(vertices.size());
				IndexedVertex &vert = vertices.emplace_back();
				vert.triangle_index = triangle_index;
				vert.position = tri.v2;
			}

			IndexedTriangle indexed_triangle;
			indexed_triangle.vertex1_index = vert1_index;
			indexed_triangle.vertex2_index = vert2_index;
			indexed_triangle.vertex3_index = vert3_index;

			triangles.push_back(indexed_triangle);

			IndexedVertex &v1 = vertices.at(vert1_index);
			IndexedVertex &v2 = vertices.at(vert2_index);
			IndexedVertex &v3 = vertices.at(vert3_index);

			v1.this_index = vert1_index;
			v1.AddIndex(vert2_index);
			v1.AddIndex(vert3_index);

			v2.this_index = vert2_index;
			v2.AddIndex(vert1_index);
			v2.AddIndex(vert3_index);

			v3.this_index = vert3_index;
			v3.AddIndex(vert1_index);
			v3.AddIndex(vert2_index);
		}
	}

	cm::NavMesh::IndexedTriangle NavMesh::FindIndexedTriangleThatContainsPoint(const Vec3f &point) const
	{
		for (const IndexedTriangle &index_triangle : triangles)
		{
			Triangle tri = IndexedTriangleToTriangle(index_triangle);

			if (IsPointInsideTriangle(tri, point))
			{
				return index_triangle;
			}
		}

		return {};
	}

	cm::Triangle NavMesh::IndexedTriangleToTriangle(const IndexedTriangle &indexed_triangle) const
	{
		Vec3f v1 = vertices.at(indexed_triangle.vertex1_index).position;
		Vec3f v2 = vertices.at(indexed_triangle.vertex2_index).position;
		Vec3f v3 = vertices.at(indexed_triangle.vertex3_index).position;

		Triangle triangle = CreateTriangle(v1, v2, v3);

		return triangle;
	}

	int32 NavMesh::FindPositionIndex(const Vec3f &position)
	{
		for (int32 index = 0; index < vertices.size(); index++)
		{
			if (position == vertices.at(index).position)
			{
				return index;
			}
		}

		return -1;
	}

	NavMesh::NavMesh()
	{

	}

	NavMesh::~NavMesh()
	{

	}



	NavAgent::NavAgent()
	{
	}

	NavAgent::~NavAgent()
	{
	}

	cm::Vec3f NavAgent::GetCurrentWapoint()
	{
		if (path.size() == 1)
		{
			return path.at(0);
		}

		return path.at(current_waypoint);
	}

	void NavAgent::PassedWaypoint()
	{
		current_waypoint++;
		if (current_waypoint == path.size())
		{
			current_waypoint--;
		}
	}

	bool NavAgent::PathFinished()
	{
		return (current_waypoint + 1) == path.size();
	}

	void NavAgent::FindPathTo(Vec3f start, Vec3f end)
	{
		current_waypoint = 1;
		path.clear();

		start.y = 0;
		end.y = 0;

		NavMesh::IndexedTriangle from = nav_mesh->FindIndexedTriangleThatContainsPoint(start);
		NavMesh::IndexedTriangle to = nav_mesh->FindIndexedTriangleThatContainsPoint(end);

		path.push_back(start);

		// @NOTE: Entity not on nav mesh
		if (from.vertex1_index == -1 || to.vertex1_index == -1)
		{
			LOG("NAVMESH, entity not on nav mesh");
			return;
		}

		// @NOTE: On the same triangle
		if (from.Equal(to)) {

			path.push_back(end);
			return;
		}


		path.push_back(end);

		std::vector<TraversalNode> closed(nav_mesh->vertices.size());
		std::queue<TraversalNode> open;

		int32 starting_index;
		{
			real32 v1dist = DistanceSqrd(nav_mesh->vertices.at(from.vertex1_index).position, start);
			real32 v2dist = DistanceSqrd(nav_mesh->vertices.at(from.vertex2_index).position, start);
			real32 v3dist = DistanceSqrd(nav_mesh->vertices.at(from.vertex3_index).position, start);
			if (v1dist < v2dist)
			{
				if (v1dist < v3dist)
				{
					starting_index = from.vertex1_index;
				}
				else
				{
					starting_index = from.vertex3_index;
				}
			}
			else
			{
				if (v2dist < v3dist)
				{
					starting_index = from.vertex2_index;
				}
				else
				{
					starting_index = from.vertex3_index;
				}
			}
		}

		TraversalNode starting_node;
		starting_node.index = starting_index;
		starting_node.parent = -1;

		open.push(starting_node);

		while (true)
		{
			if (open.size() == 0)
			{
				ASSERT(0, "NavAgent could not find path to" + ToString(end));
				return;
			}

			TraversalNode node = open.front();
			open.pop();

			if (closed.at(node.index).index == -1)
			{
				// @NOTE: Not contained in the closed list
				const NavMesh::IndexedVertex &vertex = nav_mesh->vertices.at(node.index);

				if (to.HasVertex(vertex))
				{
					while (true)
					{
						const NavMesh::IndexedVertex &path_vertex = nav_mesh->vertices.at(node.index);
						path.push_back(path_vertex.position);

						if (node.parent == -1)
						{
							break;
						}

						node = closed.at(node.parent);
					}

					path.push_back(start);

					break;
				}
				else
				{
					closed.at(node.index) = node;

					for (int32 i = 0; i < vertex.index_count; i++)
					{
						const NavMesh::IndexedVertex &adjacent_vertex = nav_mesh->vertices.at(vertex.indices[i]);
						TraversalNode adjacent_node = TraversalNode(adjacent_vertex.this_index, node.index);
						open.push(adjacent_node);
					}
				}
			}
		}

		std::reverse(path.begin(), path.end());
	}
}
