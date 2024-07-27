#pragma once

#include <typeindex>
#include <unordered_map>

class SystemBase;

class SystemManager
{
public:
	static SystemManager& Get();

	void RegisterSystem(const std::type_index& type, SystemBase* system);

	template<class T>
	T* GetSystem(const std::type_index& type) const;

	const std::unordered_map<std::type_index, SystemBase*>& GetAllSystems() const { return systems; }

private:
	std::unordered_map<std::type_index, SystemBase*> systems;
};

#define REGISTER_SYSTEM(system) \
struct system##Struct \
{ \
	system##Struct() \
	{ \
		system* ns = new system(); \
		SystemManager::Get().RegisterSystem(typeid(system), ns); \
	} \
}; \
static system##Struct system##Struct_;

template<class T>
inline T* SystemManager::GetSystem(const std::type_index& type) const
{
	const auto& it = systems.find(type);

	if (it != systems.end())
	{
		return reinterpret_cast<T*>(it->second);
	}

	return nullptr;
}
