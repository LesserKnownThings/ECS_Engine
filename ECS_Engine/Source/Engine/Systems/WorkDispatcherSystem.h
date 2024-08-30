#pragma once

#include "../Math/Numerals.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <vector>
#include <thread>

namespace LKT
{
	constexpr int32 MAX_THREAD_WORKLOAD = 5000;

	using Job = std::function<void()>;

	class WorkDispatcherSystem
	{
	public:
		static WorkDispatcherSystem& Get();

		~WorkDispatcherSystem();

		void AddTask(const Job& newTask);
		void AddTasks(std::vector<Job> tasks);

	private:
		WorkDispatcherSystem();

		void ProcessThread();

		std::vector<std::thread> workers;
		std::queue<Job> tasks;

		std::mutex mutex;
		std::condition_variable conditionalVariable;

		std::atomic<bool> isDone = false;
	};
}