#include "Grid.h"
#include "src/Debug.h"

namespace cm
{

	std::vector<cm::GridCell> Grid::GetNeighbours(const GridCell &cell)
	{
		std::vector<cm::GridCell> neighbours;

		// @NOTE: X
		if (InBounds(cell.xindex + 1, cell.yindex, cell.zindex))
			neighbours.push_back(GetCell(cell.xindex + 1, cell.yindex, cell.zindex));
		if (InBounds(cell.xindex - 1, cell.yindex, cell.zindex))
			neighbours.push_back(GetCell(cell.xindex - 1, cell.yindex, cell.zindex));

		// @NOTE: Y 
		if (InBounds(cell.xindex, cell.yindex + 1, cell.zindex))
			neighbours.push_back(GetCell(cell.xindex, cell.yindex + 1, cell.zindex));
		if (InBounds(cell.xindex, cell.yindex - 1, cell.zindex))
			neighbours.push_back(GetCell(cell.xindex, cell.yindex - 1, cell.zindex));

		// @NOTE: Z 
		if (InBounds(cell.xindex, cell.yindex, cell.zindex + 1))
			neighbours.push_back(GetCell(cell.xindex, cell.yindex, cell.zindex + 1));
		if (InBounds(cell.xindex, cell.yindex, cell.zindex - 1))
			neighbours.push_back(GetCell(cell.xindex, cell.yindex, cell.zindex - 1));

		return neighbours;
	}

	//#define LOOPTHROUGH(code) { for (int32 z = 0; z < total_depth; z++)		\
	//							{ for (int32 y = 0; y < total_height; y++)	\
	//							  { for (int32 x = 0; x < total_width; x++) \
	//								{ code }                                \
	//								} } }							        \


	void Grid::DebugDisplay()
	{
		for (GridCell &cell : cells)
		{
			Debug::Push(cell.center, 0.1f);
		}
	}

	Grid::Grid()
	{
		cells.resize(total_width * total_height * total_depth);

		for (int32 z = 0; z < total_depth; z++)
		{
			for (int32 y = 0; y < total_height; y++)
			{
				for (int32 x = 0; x < total_width; x++)
				{
					int32 index = GetIndex(x, y, z);
					GridCell &cell = cells.at(index);

					Vec3f pos;
					pos.x = x * grid_cell_width + grid_cell_width / 2.0f;
					pos.y = y * grid_cell_height + grid_cell_height / 2.0f;
					pos.z = z * grid_cell_depth + grid_cell_width / 2.0f;

					cell.center = pos;
					cell.xindex = x;
					cell.yindex = y;
					cell.zindex = z;
					cell.empty = true;
				}
			}
		}

		for (int32 x = 0; x < total_width; x++)
		{
			int32 index = GetIndex(x, 1, 0);
			GridCell &cell = cells.at(index);
			cell.empty = false;
		}

		for (int32 z = 0; z < total_depth; z++)
		{
			for (int32 x = 0; x < total_width; x++)
			{
				int32 index = GetIndex(x, 0, z);
				GridCell &cell = cells.at(index);
				cell.empty = false;
			}
		}

		for (int32 y = 0; y < total_height; y++)
		{
			for (int32 x = 0; x < total_width; x++)
			{
				int32 index = GetIndex(x, y, total_depth - 1);
				GridCell &cell = cells.at(index);
				cell.empty = false;
			}
		}

		for (int32 z = 0; z < total_depth; z++)
		{
			for (int32 y = 0; y < total_height; y++)
			{
				{
					int32 index = GetIndex(0, y, z);
					GridCell &cell = cells.at(index);
					cell.empty = false;
				}
				{
					int32 index = GetIndex(total_width - 1, y, z);
					GridCell &cell = cells.at(index);
					cell.empty = false;
				}

			}
		}
	}

	Grid::Grid(const int32 &width, const int32 &height, const int32 &depth) : total_width(width), total_height(height), total_depth(depth)
	{
		cells.resize(width * height * depth);

		for (int32 z = 0; z < total_depth; z++)
		{
			for (int32 y = 0; y < total_height; y++)
			{
				for (int32 x = 0; x < total_width; x++)
				{
					int32 index = GetIndex(x, y, z);
					GridCell &cell = cells.at(index);

					Vec3f pos;
					pos.x = x * grid_cell_width + grid_cell_width / 2.0f;
					pos.y = y * grid_cell_height + grid_cell_height / 2.0f;
					pos.z = z * grid_cell_depth + grid_cell_width / 2.0f;

					cell.center = pos;
					cell.xindex = x;
					cell.yindex = y;
					cell.zindex = z;
					cell.empty = true;
				}
			}
		}
		for (int32 z = 0; z < total_depth; z++)
		{
			for (int32 x = 0; x < total_width; x++)
			{
				int32 index = GetIndex(x, 0, z);
				GridCell &cell = cells.at(index);
				cell.empty = false;
			}
		}
	}

	Grid::~Grid()
	{

	}

}