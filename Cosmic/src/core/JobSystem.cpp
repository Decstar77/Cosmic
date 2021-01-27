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
			threads.at(i).index = i;
			threads.at(i).working.store(false);
			threads.at(i).thread = std::thread(&JobSystem::ThreadFunc, this, i);
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

		for (int32 i = 0; i < threads.size(); i++)
		{
			if (threads.at(i).thread.joinable())
				threads.at(i).thread.join();
		}

		delete[] work;
	}

	bool JobSystem::HasThreadsWorking()
	{
		for (int32 i = 0; i < threads.size(); i++)
		{
			if (threads.at(i).working.load())
				return true;
		}

		return false;
	}

	void JobSystem::ThreadFunc(int32 index)
	{
		JobThread *me = &threads.at(index);

		while (run)
		{
			JobWork* work = work_queue.Take();

			me->working.store(true);
			work->Process();
			me->working.store(false);
		}
	}
}