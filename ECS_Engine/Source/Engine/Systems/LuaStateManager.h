#pragma once

#include "lua/lua.hpp"

#include <cstdint>
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