#pragma once
#include "src/Cosmic.h"

namespace cm
{
	template<typename T>
	class MemoryAreana
	{
	public:
		int32 count;
		std::vector<T> data;
		std::queue<uint32> freelist;

	public:
		uint32 Add(const T &t)
		{
			if (freelist.size() > 0)
			{
				uint32 index = freelist.front(); freelist.pop();
				data.at(index) = t;
				count++;

				return index;
			}
			else
			{
				uint32 index = data.size();
				data.emplace_back(t);
				count++;

				return index;
			}
		}

		T* Remove(const uint32 &index)
		{
			freelist.push(index);

			return &data.at(index);
		}

	public:

		MemoryAreana()
			: count(0)
		{
		}

		MemoryAreana(const uint32 &size)
			: count(0)
		{
			data.reserve(size);
		}

		~MemoryAreana() {}
	};
}