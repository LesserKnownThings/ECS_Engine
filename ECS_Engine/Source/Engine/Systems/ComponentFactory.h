#pragma once

#include <cstdint>
#include <functional>
#include <tuple>

namespace LKT
{
	struct ComponentResource
	{
		uint32_t bufferSize;
		void* buffer;
	};

	class ComponentFactory
	{
		virtual void CreateComponentData() = 0;
	};

	template<typename... Args>
	class ComponentFactoryType : public ComponentFactory
	{
		using FactoryFunc = std::function<void(Args...)>;

	public:
		ComponentFactoryType(FactoryFunc func, std::tuple<Args...> args)
			: creationFunc(func), storedArgs(args)
		{}

		void CreateComponentData() override
		{
			std::apply(creationFunc, storedArgs);
		}

	private:
		FactoryFunc creationFunc;
		std::tuple<Args...> storedArgs;
	};

	template<>
	class ComponentFactoryType<> : public ComponentFactory
	{
		using FactoryFunc = std::function<void()>;

	public:
		ComponentFactoryType(FactoryFunc func)
			: creationFunc(func) {}

		void CreateComponentData() override
		{
			creationFunc();
		}

	private:
		FactoryFunc creationFunc;
	};
}