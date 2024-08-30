#include "TaskManagerSystem.h"

namespace LKT
{
	TaskManagerSystem& TaskManagerSystem::Get()
	{
		static TaskManagerSystem instance;
		return instance;
	}

	void TaskManagerSystem::ExecuteTasks(int32_t handle)
	{
		for (auto it = tasks.begin(); it != tasks.end();)
		{
			ITask* task = *it;
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