#pragma once

#include "glm/glm.hpp"
#include "UniqueID.h"
#include "ParticleShapeData.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <variant>

struct lua_State;

namespace LKT
{
	class ParticleEmitterShape;

	class ParticleEmitterShapeManager
	{
	public:
		static ParticleEmitterShapeManager& Get();

		bool GetShapeData(const ParticleShapeData& emitter, glm::vec3& outPosition, glm::vec3& outVelocity);
		bool GetEmitterTypeData(const std::string& emitterName, ParticleShapeData& outEmitterData) const;

		const std::unordered_map<std::string, ParticleShapeData>& GetShapes() const { return shapes; }

	private:
		ParticleEmitterShapeManager();

		void LoadShapes();
		void LoadRefs(int32_t& paramsRef, int32_t& funcRef);

		bool Internal_GetShapeData(const ParticleShapeData& shapeData, glm::vec3& outPosition, glm::vec3& outVelocity);		

		std::unordered_map<std::string, ParticleShapeData> shapes;

		lua_State* L = nullptr;
	};
}