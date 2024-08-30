#include "LuaStateManager.h"

#include <iostream>

namespace LKT
{
	namespace
	{
		static void LuaError(lua_State* L)
		{
			const char* errorMessage = lua_tostring(L, -1);
			printf("Error: %s\n", errorMessage);
			lua_pop(L, 1);
		}
	}

	LuaStateManager::~LuaStateManager()
	{
		lua_close(L);
	}

	LuaStateManager& LuaStateManager::Get()
	{
		static LuaStateManager instance;
		return instance;
	}

	//bool LuaStateManager::CallFunc(int32_t params, int32_t results)
	//{
	//	if (lua_pcall(L, params, results, 0) == LUA_OK)
	//	{
	//		currentIndex--;
	//		return true;
	//	}

	//	LKT::LuaError(L);
	//	return false;
	//}

	//bool LuaStateManager::DoFile(const char* fileName)
	//{
	//	if (luaL_dofile(L, fileName) == LUA_OK)
	//	{
	//		return true;
	//	}
	//	 
	//	LKT::LuaError(L);
	//	return false;
	//}

	//bool LuaStateManager::Next(int32_t startIndex) const
	//{
	//	if (lua_next(L, startIndex) != 0)
	//	{
	//		return true;
	//	}

	//	return false;
	//}

	//int32_t LuaStateManager::GetLength() const
	//{
	//	return luaL_len(L, currentIndex);
	//}

	//std::string LuaStateManager::ToString(int32_t overrideIndex) const
	//{
	//	const int32_t index = overrideIndex == 0 ? currentIndex : overrideIndex;

	//	return lua_tostring(L, index);
	//}

	//bool LuaStateManager::ToBool(int32_t overrideIndex) const
	//{
	//	const int32_t index = overrideIndex == 0 ? currentIndex : overrideIndex;

	//	return lua_toboolean(L, index);
	//}

	//bool LuaStateManager::IsNumber() const
	//{
	//	if (lua_isnumber(L, currentIndex))
	//	{
	//		return true;
	//	}

	//	LKT::LuaError(L);
	//	return false;
	//}

	//bool LuaStateManager::IsTable() const
	//{
	//	if (lua_istable(L, currentIndex))
	//	{
	//		return true;
	//	}

	//	LKT::LuaError(L);
	//	return false;
	//}

	//bool LuaStateManager::IsFunc() const
	//{
	//	if (lua_isfunction(L, currentIndex))
	//	{
	//		return true;
	//	}

	//	LKT::LuaError(L);
	//	return false;
	//}

	//void LuaStateManager::GetGlobal(const char* name)
	//{
	//	if (lua_getglobal(L, name))
	//	{
	//		currentIndex--;
	//	}
	//	else
	//	{
	//		LKT::LuaError(L);
	//	}
	//}

	//void LuaStateManager::GetTable(int32_t index)
	//{
	//	lua_gettable(L, index);
	//}

	//void LuaStateManager::PushInt(int32_t n)
	//{
	//	lua_pushinteger(L, n);
	//	currentIndex--;
	//}

	//void LuaStateManager::PushNull()
	//{
	//	lua_pushnil(L);
	//	currentIndex--;
	//}

	//void LuaStateManager::Pop(int32_t n)
	//{
	//	lua_pop(L, n);
	//	currentIndex += n;

	//	currentIndex = std::min(0, currentIndex);
	//}

	LuaStateManager::LuaStateManager()
	{
		L = luaL_newstate();

		luaL_openlibs(L);
	}
}