#pragma once

#include "../Math/Numerals.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <vector>
#include <thread>

constexpr int32 MAX_THREAD_WORKLOAD = 5000;

using Task = std::function<void()>;

class WorkDispatcherSystem
{
public:
	static WorkDispatcherSystem& Get();

	~WorkDispatcherSystem();

	void AddTask(const Task& newTask);
	void AddTasks(std::vector<Task> tasks);

private:
	WorkDispatcherSystem();

	void ProcessThread();

	std::vector<std::thread> workers;
	std::queue<Task> tasks;

	std::mutex mutex;
	std::condition_variable conditionalVariable;

	std::atomic<bool> isDone = false;
};