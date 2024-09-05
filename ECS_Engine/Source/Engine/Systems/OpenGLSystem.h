#pragma once

#ifndef OPEN_GL_SYSTEM
#define OPEN_GL_SYSTEM

#include "Entity.h"
#include "RenderComponent.h"
#include "ResourceManagerSystem.h"
#include "SDL/SDL_video.h"
#include "SystemBase.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace LKT
{
	class CameraSystem;
	class ParticleSystem;

	struct DrawData;

	class OpenGLSystem : public SystemBase
	{
	public:
		~OpenGLSystem() = default;
		OpenGLSystem();

		static OpenGLSystem& Get();

		bool InitializeSystem(int32_t inWidth, int32_t inHeight);
		void Uninitialize();

		void Render();

		uint32_t CreateComponent(const Entity& e, void* componentData) override;
		void CreateComponents(int32_t entityCount, Entity* entities, void* componentData) override;

		bool GetComponent(const Entity& e, uint32_t& outComponent) const;

		void SetComponentMesh(const std::string& meshPath, const Entity& e);

	private:
		void PreRender();
		void PreRenderUI();
		void PostRenderUI();
		void PostRender();

		void HandlePostShaderCompile();

		void AllocateMemory(int32_t size);

		void LoadMeshData(uint32_t componentIndex, const DrawData& tempData);

		void HandleWindowResized();
		void SetupUI();

		SDL_Window* window = nullptr;
		SDL_GLContext glContext = nullptr;

		int32_t width = 0;
		int32_t height = 0;

		std::string defaultShaderName = "Unlit";

		CameraSystem* defaultCamera = nullptr;

		RenderComponent renderComponent;
		std::unordered_map<Entity, uint32_t> instances;
	};

	REGISTER_COMPONENT(OpenGLSystem, RenderComponentResource)
}
#endif