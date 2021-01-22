#pragma once

#include "src/Cosmic.h"
#include "src/util/BlockingQueue.h"

namespace cm
{
	class JobWork
	{
	public:
		std::string name;

	public:
		JobWork(const String name) : name(name) {}

		virtual void Process() = 0;
	};

	class JobSystem
	{
		bool run = true;
		std::vector<std::thread> threads;
		BlockingQueue<JobWork*> work_queue;

	public:
		JobSystem();
		~JobSystem();

		inline void AddJob(JobWork *work)
		{
			work_queue.Push(work);
		}

		inline bool HasWork()
		{
			return work_queue.Empty();
		}

		inline static JobSystem* GetInstance() { return instance; }

	private:
		inline static JobSystem *instance = nullptr;

		class DummyWork : public JobWork
		{
		public:
			DummyWork() : JobWork("Dummy Work") {}

			virtual void Process() override {};
		};

	private:
		void ThreadFunc();

	};
}
