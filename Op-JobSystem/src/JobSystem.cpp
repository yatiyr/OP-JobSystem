#include <iostream>

#include <JobSystem.h>

#define EXIT_THREAD -1
#define PRINT 1

namespace OP
{
	struct JobSystemData
	{
		uint32_t numThreads = 0;

		ThreadSafeQueue<std::pair<std::function<void()>, uint32_t*>, 512> jobPool;

		std::condition_variable wakeCondition;
		std::mutex wakeMutex;

		uint64_t currentLabel;
		std::atomic<uint64_t> finishedLabel;
	};
	

	static JobSystemData s_JobSystemData;

	inline void JobSystem::Poll()
	{
		// Wake one of the threads up
		s_JobSystemData.wakeCondition.notify_one();
		std::this_thread::yield();
	}


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
				uint32_t* isFinished;

				std::pair<std::function<void()>, uint32_t*> pair = { job, isFinished };


				while (true)
				{
					if (s_JobSystemData.jobPool.Pop(pair))
					{
						pair.first(); // execute found job
						*(pair.second) -= 1;
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

	void JobSystem::Execute(const std::function<void()>& job, JobTracker& tracker)
	{

		// Update main thread label state
		s_JobSystemData.currentLabel += 1;

		std::pair<std::function<void()>, uint32_t*> pair = { job, &tracker.isFinished };

		// Try to push a new job
		while (!s_JobSystemData.jobPool.Push(pair))
			Poll();

		s_JobSystemData.wakeCondition.notify_one();

	}


	bool JobSystem::IsBusy()
	{
		return s_JobSystemData.finishedLabel.load() < s_JobSystemData.currentLabel;
	}

	void JobSystem::Wait()
	{
		while (IsBusy()) { Poll(); }
	}


	/*void JobSystem::Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(JobDispatchArgs)>& job)
	{
		if (jobCount == 0 || groupSize == 0)
		{
			return;
		}

		const uint32_t groupCount = (jobCount + groupSize - 1) / groupSize;

		s_JobSystemData.currentLabel += groupCount;

		for (uint32_t groupIndex = 0; groupIndex < groupCount; groupIndex++)
		{
			auto& jobGroup = [jobCount, groupSize, job, groupIndex]()
			{
				// Calculate the current group's offset into the jobs:
				const uint32_t groupJobOffset = groupIndex * groupSize;
				const uint32_t groupJobEnd = std::min(groupJobOffset + groupSize, jobCount);

				JobDispatchArgs args;
				args.groupIndex = groupIndex;

				// Inside the group, loop through all job indices and execute job
				for (uint32_t i = groupJobOffset; i < groupJobEnd; i++)
				{
					args.jobIndex = i;
					job(args);
				}
			};

			// Try to push a new job until it is pushed successfully
			while (!s_JobSystemData.jobPool.Push(jobGroup)) { Poll(); }

			s_JobSystemData.wakeCondition.notify_one();
		}

	} */

}