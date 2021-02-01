#pragma once

#include "src/Cosmic.h"

namespace cm
{
	enum class DataType
	{
		INAVLID = 0,
		FLOAT,
		FLOAT2,
		FLOAT3,
		FLOAT4,
		MAT3,
		MAT4,
		INT,
		INT2,
		INT3,
		INT4,
		BOOL
	};

	inline int32 GetDataTypeSizeInBytes(const DataType &type)
	{
		switch (type)
		{
		case DataType::FLOAT:    return 4;
		case DataType::FLOAT2:   return 4 * 2;
		case DataType::FLOAT3:   return 4 * 3;
		case DataType::FLOAT4:   return 4 * 4;
		case DataType::MAT3:     return 4 * 3 * 3;
		case DataType::MAT4:     return 4 * 4 * 4;
		case DataType::INT:      return 4;
		case DataType::INT2:     return 4 * 2;
		case DataType::INT3:     return 4 * 3;
		case DataType::INT4:     return 4 * 4;
		case DataType::BOOL:     return 1;
		}

		return 0;
	}

	inline int32 GetDataTypeComponentCount(const DataType &type)
	{
		switch (type)
		{
		case DataType::FLOAT:    return 1;
		case DataType::FLOAT2:   return 1 * 2;
		case DataType::FLOAT3:   return 1 * 3;
		case DataType::FLOAT4:   return 1 * 4;
		case DataType::MAT3:     return 1 * 3 * 3;
		case DataType::MAT4:     return 1 * 4 * 4;
		case DataType::INT:      return 1;
		case DataType::INT2:     return 1 * 2;
		case DataType::INT3:     return 1 * 3;
		case DataType::INT4:     return 1 * 4;
		case DataType::BOOL:     return 1;
		}

		return 0;
	}

	class BufferLayout
	{
	public:
		void Next();
		void Reset();

		//uint32 GetTotalComponentCount();
		//uint32 GetTotalAttributeCount();

		uint32 GetCurrentOffSet();
		uint32 GetCurrentSizeInBytes();
		uint32 GetCurrentComponentAttribCount();

		//uint32 GetStride();
		//uint32 GetTotalSize();
		//uint32 GetSizeOf();
		//uint32 GetAttributeDivisor();

		//DataType GetCurrentShaderType();

	public:
		BufferLayout();
		BufferLayout(const std::vector<DataType> &layout);
		~BufferLayout();

	private:
		uint32 stride = 0;
		uint32 current_offset = 0;
		uint32 current_next = 0;

		std::vector<DataType> layout;
	};
}
