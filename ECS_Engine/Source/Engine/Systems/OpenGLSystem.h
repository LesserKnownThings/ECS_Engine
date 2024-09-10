#pragma once

#ifndef OPEN_GL_SYSTEM
#define OPEN_GL_SYSTEM

#include "Delegate.h"
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
	DECLARE_DELEGATE_TwoParams(OnWindowResizedParams, int32_t, int32_t);

	class Camera;
	class ParticleSystem;

	struct MeshData;

	class OpenGLSystem : public SystemBase
	{
	public:
		~OpenGLSystem() = default;
		OpenGLSystem();

		static OpenGLSystem &Get();

		bool InitializeSystem(int32_t inWidth, int32_t inHeight);
		void Uninitialize();

		void Render();

		uint32_t CreateComponent(const Entity &e, void *componentData) override;
		void CreateComponents(int32_t entityCount, Entity *entities, void *componentData) override;

		void ResetViewport();

		bool GetComponent(const Entity &e, uint32_t &outComponent) const;
		static void GetViewportSize(float &x, float &y);

		static void GetRenderComponentObject(const MeshData &meshData, RenderComponentObject &rco);
		static void UnloadRenderComponentObject(RenderComponentObject &rco);

		void SetComponentMesh(const std::string &meshPath, const Entity &e);

		OnWindowResizedParams onWindowResized;

	private:
		void PreRender();
		void PreRenderUI();
		void PostRenderUI();
		void PostRender();

		void HandlePostShaderCompile();

		void AllocateMemory(int32_t size);

		void LoadMeshData(uint32_t componentIndex, const MeshData &tempData);

		void HandleWindowResized();
		void SetupUI();

		SDL_Window *window = nullptr;
		SDL_GLContext glContext = nullptr;

		int32_t width = 0;
		int32_t height = 0;

		std::string defaultShaderName = "Unlit";

		RenderComponent renderComponent;
		std::unordered_map<Entity, uint32_t> instances;
	};

	REGISTER_COMPONENT(OpenGLSystem, RenderComponentResource)
}
#endif