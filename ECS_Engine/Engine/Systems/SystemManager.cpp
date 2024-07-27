#include "SystemManager.h"

SystemManager& SystemManager::Get()
{
	static SystemManager instance;
	return instance;
}

void SystemManager::RegisterSystem(const std::type_index& type, SystemBase* system)
{
	systems[type] = system;
}