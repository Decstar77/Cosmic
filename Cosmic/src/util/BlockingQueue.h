#pragma once
#include "src/Cosmic.h"
#include "src/util/RetainLock.h"

namespace cm
{
	template<typename T>
	class BlockingQueue
	{
	public:
		void Push(T const& _data)
		{
			{
				std::lock_guard<std::mutex> lock(guard);
				queue.push(_data);
			}
			signal.notify_one();
		}

		bool Empty() const
		{
			std::lock_guard<std::mutex> lock(guard);
			return !queue.empty();
		}

		T Take()
		{
			std::unique_lock<std::mutex> lock(guard);
			while (queue.empty())
			{
				signal.wait(lock);
			}

			T t = queue.front();
			queue.pop();

			return t;
		}

	private:
		std::queue<T> queue;
		mutable std::mutex guard;
		std::condition_variable signal;
	};
};
