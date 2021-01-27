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
	private:
		class JobThread
		{
		public:
			int32 index;
			std::thread thread;
			AtomicBool working;

		public:
			JobThread() : working() {}
			JobThread(const JobThread &other) : working(other.working.load()) {}
			JobThread &operator=(const JobThread &other) { working.store(other.working.load()); }
		};

	public:
		bool run = true;
		std::vector<JobThread> threads;
		BlockingQueue<JobWork*> work_queue;

	public:
		JobSystem();
		~JobSystem();

		inline void AddJob(JobWork *work)
		{
			work_queue.Push(work);
		}

		inline bool HasWork() { return !work_queue.Empty(); }

		bool HasThreadsWorking();

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
		void ThreadFunc(int32 index);

	};
}
