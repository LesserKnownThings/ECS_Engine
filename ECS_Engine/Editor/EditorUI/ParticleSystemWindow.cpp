#include "ParticleSystemWindow.h"
#include "imgui/imgui.h"
#include "Systems/ParticleSystem/ParticleEmitter.h"
#include "Systems/ParticleSystem/ParticleEmitterShapeManager.h"
#include "Systems/TaskManagerSystem.h"

namespace LKT
{
	void ParticleSystemWindow::Initialize(const std::string &inName)
	{
		EngineWindow::Initialize(inName);

		/*ParticleEmitterShapeManager& sm = ParticleEmitterShapeManager::Get();

		for (const auto& shape : sm.GetShapes())
		{
			items.push_back(shape.first.c_str());

			ShapeUIData uiData{ shape.first, shape.second.params };

			data.push_back(uiData);
		}*/
	}

	void ParticleSystemWindow::Uninitialize()
	{
		EngineWindow::Uninitialize();
	}

	void ParticleSystemWindow::RenderContent()
	{
		if (selectedInstance != nullptr)
		{
			if (ImGui::Combo("Shapes", &selectedIndex, items.data(), items.size()))
			{
				if (selectedIndex != previousIndex)
				{
					previousIndex = selectedIndex;
					hasDataChanged = true;
				}
			}

			DrawAndUpdateParams();

			if (ImGui::Button("Refresh") && hasDataChanged)
			{
				hasDataChanged = false;
			}
		}
	}

	void ParticleSystemWindow::DrawAndUpdateParams()
	{
		// Generic params

		// Shape params
		for (auto &param : data[selectedIndex].params)
		{
			if (std::holds_alternative<float>(param.second))
			{
				float currentValue = std::get<float>(param.second);

				if (ImGui::InputFloat(param.first.c_str(), &currentValue))
				{
					data[selectedIndex].params[param.first] = currentValue;
					hasDataChanged = true;
				}
			}
			else if (std::holds_alternative<bool>(param.second))
			{
				bool currentValue = std::get<bool>(param.second);

				if (ImGui::Checkbox(param.first.c_str(), &currentValue))
				{
					data[selectedIndex].params[param.first] = currentValue;
					hasDataChanged = true;
				}
			}
		}
	}
}