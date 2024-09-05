#include "ParticleEmitterShapeManager.h"
#include "FileHelper.h"
#include "lua/lua.hpp"

#include <vector>

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

	ParticleEmitterShapeManager& ParticleEmitterShapeManager::Get()
	{
		static ParticleEmitterShapeManager instance;
		return instance;
	}

	bool ParticleEmitterShapeManager::Internal_GetShapeData(const ParticleShapeData& shapeData, glm::vec3& outPosition, glm::vec3& outVelocity)
	{
		float pos[3] = { 0, 0, 0 };
		float vel[3] = { 0, 0, 0 };

		//Override the params first
		lua_rawgeti(L, LUA_REGISTRYINDEX, shapeData.paramsRef);

		for (const auto& it : shapeData.params)
		{
			lua_pushstring(L, it.first.c_str());

			if (const float* value = std::get_if<float>(&it.second))
			{
				lua_pushnumber(L, *value);
				lua_settable(L, -3);
			}
		}

		lua_setglobal(L, "params");

		lua_rawgeti(L, LUA_REGISTRYINDEX, shapeData.funcRef);

		if (lua_pcall(L, 0, 2, 0) == LUA_OK)
		{
			//Vel
			if (lua_istable(L, -1))
			{
				int32_t len = luaL_len(L, -1);
				for (int32_t i = 1; i <= len; ++i)
				{
					lua_pushinteger(L, i);
					lua_gettable(L, -2);

					vel[i - 1] = lua_tonumber(L, -1);

					lua_pop(L, 1);
				}
			}

			lua_pop(L, 1);

			//Pos
			if (lua_istable(L, -1))
			{
				int32_t len = luaL_len(L, -1);
				for (int32_t i = 1; i <= len; ++i)
				{
					lua_pushinteger(L, i);
					lua_gettable(L, -2);

					pos[i - 1] = lua_tonumber(L, -1);
					
					lua_pop(L, 1);
				}
			}

			lua_pop(L, 1);
		}
		else
		{
			LuaError(L);
			return false;
		}

		outPosition = glm::vec3(pos[0], pos[1], pos[2]);
		outVelocity = glm::vec3(vel[0], vel[1], vel[2]);
		return true;
	}

	bool ParticleEmitterShapeManager::GetShapeData(const ParticleShapeData& shapeData, glm::vec3& outPosition, glm::vec3& outVelocity)
	{
		return Internal_GetShapeData(shapeData, outPosition, outVelocity);
	}

	bool ParticleEmitterShapeManager::GetEmitterTypeData(const std::string& emitterName, ParticleShapeData& outEmitterData) const
	{
		auto it = shapes.find(emitterName);

		if (it != shapes.end())
		{
			outEmitterData = it->second;
			return true;
		}

		return false;
	}

	ParticleEmitterShapeManager::ParticleEmitterShapeManager()
	{
		L = luaL_newstate();
		luaL_openlibs(L);

		LoadShapes();
	}

	void ParticleEmitterShapeManager::LoadShapes()
	{
		std::vector<std::string> files;
		FileHelper::GetFilesFromDirectory("LuaScripts/ParticleShapes", files);

		for (const std::string& file : files)
		{
			ParticleShapeData psd;

			if (luaL_dofile(L, file.c_str()) == LUA_OK)
			{
				std::string displayName;

				lua_getglobal(L, "display_name");

				displayName = lua_tostring(L, -1);
				psd.shapeName = displayName;

				lua_pop(L, 1);

				lua_getglobal(L, "params");

				if (lua_istable(L, -1))
				{
					lua_pushnil(L);

					while (lua_next(L, -2))
					{
						std::string key;
						ShapeDataVariant value;

						key = lua_tostring(L, -2);

						if (lua_isboolean(L, -1))
						{
							value = static_cast<bool>(lua_toboolean(L, -1));
						}
						else if (lua_isnumber(L, -1))
						{
							value = static_cast<float>(lua_tonumber(L, -1));
						}

						psd.params[key] = value;

						lua_pop(L, 1);
					}
				}

				LoadRefs(psd.paramsRef, psd.funcRef);
				lua_pop(L, 1);

				shapes.emplace(displayName, psd);
			}
		}
	}

	void ParticleEmitterShapeManager::LoadRefs(int32_t& paramsRef, int32_t& funcRef)
	{
		//Need to ref both the params and the func
		lua_getglobal(L, "params");
		if (lua_istable(L, -1))
		{
			paramsRef = luaL_ref(L, LUA_REGISTRYINDEX);
		}

		lua_getglobal(L, "get_shape_data");		
		if (lua_isfunction(L, -1))
		{
			funcRef = luaL_ref(L, LUA_REGISTRYINDEX);
		}
	}
}