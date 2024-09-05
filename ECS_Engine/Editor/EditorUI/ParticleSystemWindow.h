#pragma once

#include "Systems/ParticleSystem/ParticleShapeData.h"
#include "UI/EngineWindow.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace LKT
{
	class ParticleEmitter;

	struct ShapeUIData
	{
		ShapeUIData(const std::string &inName, const std::unordered_map<std::string, ShapeDataVariant> &inParams)
			: shapeName(inName), params(inParams)
		{
		}

		std::string shapeName;
		std::unordered_map<std::string, ShapeDataVariant> params;
	};

	class ParticleSystemWindow : public EngineWindow
	{
	public:
		virtual void Initialize(const std::string &inName) override;
		virtual void Uninitialize() override;

		virtual void RenderContent() override;

	private:
		void DrawAndUpdateParams();

		std::vector<const char *> items;
		std::vector<ShapeUIData> data;

		int selectedIndex = 0;
		int previousIndex = -1;

		bool hasDataChanged = false;

		ParticleEmitter *selectedInstance = nullptr;
	};

	REGISTER_WINDOW(ParticleSystemWindow, "Tools/Particle System")
}