#include "JobSystem.h"

namespace cm
{
	JobSystem::JobSystem()
	{
		int32 processor_count = std::thread::hardware_concurrency();

		LOG("Starting " << processor_count - 2 << " threads");

		threads.resize(processor_count - 2);
		instance = this;
		for (int32 i = 0; i < threads.size(); i++)
		{
			threads.at(i) = std::thread(&JobSystem::ThreadFunc, this);
		}
	}

	JobSystem::~JobSystem()
	{
		run = false;

		DummyWork *work = new DummyWork[100];

		for (int i = 0; i < 100; i++)
		{
			work_queue.Push(&work[i]);
		}

		for (std::thread &thread : threads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}

		delete[] work;
	}

	void JobSystem::ThreadFunc()
	{
		while (run)
		{
			JobWork* work = work_queue.Take();
			work->Process();
		}
	}
}