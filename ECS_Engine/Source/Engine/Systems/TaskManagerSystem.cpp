#include "TaskManagerSystem.h"

namespace LKT
{
	TaskManagerSystem &TaskManagerSystem::Get()
	{
		static TaskManagerSystem instance;
		return instance;
	}

	void TaskManagerSystem::ExecuteTasks(std::optional<int32_t> handle)
	{
		for (auto it = tasks.begin(); it != tasks.end();)
		{
			ITask *task = *it;

			if (handle != std::nullopt && task->customHandle != handle)
			{
				++it;
				continue;
			}

			if (task->isAlive)
			{
				task->Execute();
				++it;
			}
			else
			{
				it = tasks.erase(it);
			}
		}
	}
}