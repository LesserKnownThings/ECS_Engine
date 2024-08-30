#include "WorkDispatcherSystem.h"
#include "DebugSystem.h"

namespace LKT
{
	WorkDispatcherSystem& WorkDispatcherSystem::Get()
	{
		static WorkDispatcherSystem instance;
		return instance;
	}

	WorkDispatcherSystem::~WorkDispatcherSystem()
	{
		isDone = true;
		conditionalVariable.notify_all();

		for (std::thread& worker : workers)
		{
			if (worker.joinable())
			{
				worker.join();
			}
		}
	}

	WorkDispatcherSystem::WorkDispatcherSystem()
	{
		for (uint32_t i = 0; i < std::thread::hardware_concurrency(); ++i)
		{
			workers.emplace_back(&WorkDispatcherSystem::ProcessThread, this);
		}
	}

	void WorkDispatcherSystem::AddTask(const Job& newTask)
	{
		std::lock_guard<std::mutex> lock(mutex);
		tasks.push(newTask);
		conditionalVariable.notify_one();
	}

	void WorkDispatcherSystem::AddTasks(std::vector<Job> tasks)
	{
		std::lock_guard<std::mutex> lock(mutex);

		for (Job& task : tasks)
		{
			this->tasks.push(std::move(task));
		}

		conditionalVariable.notify_all();
	}

	void WorkDispatcherSystem::ProcessThread()
	{
		while (!isDone)
		{
			Job task;
			{
				std::unique_lock<std::mutex> uniqueLock(mutex);
				conditionalVariable.wait(uniqueLock, [this]
					{
						return !tasks.empty() || isDone;
					});

				if (isDone && tasks.empty())
				{
					return;
				}

				task = std::move(tasks.front());
				tasks.pop();
			}

			if (task)
			{
				task();
			}
		}
	}
}