#pragma once

#include <cstdint>
#include <lua.hpp>
#include <string>

namespace LKT
{
	class LuaStateManager
	{
	public:
		~LuaStateManager();

		static LuaStateManager& Get();

	private:
		LuaStateManager();

		lua_State* L = nullptr;
	};
}