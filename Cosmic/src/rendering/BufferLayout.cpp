#include "BufferLayout.h"

namespace cm
{


	void BufferLayout::Next()
	{
		current_offset += GetDataTypeSizeInBytes(layout.at(current_next));
		current_next++;
	}

	void BufferLayout::Reset()
	{
		current_next = 0;
		current_offset = 0;
	}

	uint32 BufferLayout::GetCurrentOffSet()
	{
		return current_offset;
	}

	uint32 BufferLayout::GetCurrentSizeInBytes()
	{
		return GetDataTypeSizeInBytes(layout.at(current_next));
	}

	uint32 BufferLayout::GetCurrentComponentAttribCount()
	{
		return GetDataTypeComponentCount(layout.at(current_next));
	}

	BufferLayout::BufferLayout()
	{

	}

	BufferLayout::BufferLayout(const std::vector<DataType> &layout)
	{
		for (const DataType &type : layout)
		{
			stride += GetDataTypeSizeInBytes(type);
		}

		this->layout = layout;
	}

	BufferLayout::~BufferLayout()
	{

	}

}