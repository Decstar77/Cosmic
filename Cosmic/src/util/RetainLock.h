#pragma once
#include "src/Cosmic.h"
#include <mutex>
#include <optional>
#include <condition_variable>

namespace cm
{
	class RetainLock
	{
	public:
		void Lock();
		void Unlock();
		bool IsLocked();

	public:
		RetainLock() : locked(false) {}
		~RetainLock() {}

	private:
		std::atomic<bool> locked;
		std::mutex mx;
		std::condition_variable cv;
		std::unique_lock<std::mutex> lk;
	};
}