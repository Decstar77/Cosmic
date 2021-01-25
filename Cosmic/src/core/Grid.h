#pragma once

#include "src/Cosmic.h"
#include "src/math/CosmicMath.h"
#include "src/math/CosmicGeometry.h"

namespace cm
{
	struct GridCell
	{
		bool empty = true;
		Vec3f center;

		int32 xindex;
		int32 yindex;
		int32 zindex;
	};

	class Grid
	{
	public:
		int32 total_width = 12;
		int32 total_height = 5;
		int32 total_depth = 8;

		real32 grid_cell_width = 2.05f;
		real32 grid_cell_height = 2.05f;
		real32 grid_cell_depth = 2.05f;

		real32 grid_cell_half_width = grid_cell_width / 2.0f;
		real32 grid_cell_half_height = grid_cell_height / 2.0f;
		real32 grid_cell_half_depth = grid_cell_depth / 2.0f;


		std::vector<GridCell> cells;

	public:
		inline int32 GetIndex(const int32 &x, const int32 &y, const int32 &z) const
		{
			return (z * total_width * total_height) + (y * total_width) + x;
		}

		inline Vec3f GetPosition(int32 index) const
		{
			int32 z = index / (total_width * total_height);
			index -= (z * total_width * total_height);
			int32 y = index / total_width;
			int32 x = index % total_width;

			return Vec3f((real32)x, (real32)y, (real32)z);
		}

		inline GridCell GetCell(const int32 &x, const int32 &y, const int32 &z) const
		{
			return cells.at(GetIndex(x, y, z));
		}

		inline GridCell GetCell(const int32 &index) const
		{
			return cells.at(index);
		}

		inline bool InBounds(const int32 &x, const int32 &y, const int32 &z)
		{
			return x >= 0 && x < total_width &&
				y >= 0 && y < total_height &&
				z >= 0 && z < total_depth;
		}

		std::vector<GridCell> GetNeighbours(const GridCell &cell);

		void DebugDisplay();

	public:

		Grid();
		Grid(const int32 &width, const int32 &height, const int32 &depth);
		~Grid();



	};


}
