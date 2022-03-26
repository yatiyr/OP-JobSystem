#include <iostream>

#include <JobSystem.h>

#define EXIT_THREAD -1
#define PRINT 1

namespace OP
{
	struct JobSystemData
	{
		uint32_t numThreads = 0;

		ThreadSafeQueue<std::function<void()>, 512> jobPool;

		std::condition_variable wakeCondition;
		std::mutex wakeMutex;

		uint64_t currentLabel;
		std::atomic<uint64_t> finishedLabel;
	};
	

	static JobSystemData s_JobSystemData;


	void JobSystem::Initialize()
	{

		// Worker execution state is 0 when initializing
		s_JobSystemData.finishedLabel.store(0);

		// Get number of cores for this system
		auto numCores = std::thread::hardware_concurrency();

		// Actual number of threads we want
		s_JobSystemData.numThreads = std::max(1u, numCores);

		// Create worker threads
		for (uint32_t threadID = 0; threadID < s_JobSystemData.numThreads; threadID++)
		{
			std::thread worker([] {
				std::function<void()> job; // current job

				while (true)
				{
					if (s_JobSystemData.jobPool.Pop(job))
					{
						job(); // execute found job
						s_JobSystemData.finishedLabel.fetch_add(1); // update worker label state
					}
					else
					{
						// no job, put thread to sleep
						std::unique_lock<std::mutex> lock(s_JobSystemData.wakeMutex);
						s_JobSystemData.wakeCondition.wait(lock);
					}
				}
				});

			worker.detach();
		}
	}


}