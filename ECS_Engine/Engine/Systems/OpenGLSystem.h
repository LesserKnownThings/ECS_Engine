#pragma once

#include "../Entity.h"
#include "SDL_video.h"
#include "SystemBase.h"
#include "SystemManager.h"
#include "ResourceManagerSystem.h"

#include <string>
#include <unordered_map>

class CameraSystem;
class ShaderProgram;

struct DrawData;

class OpenGLSystem : public SystemBase
{
	struct RenderComponentData
	{
		int32 instancesCount;
		int32 allocatedInstances;

		void* buffer;

		Entity* entity;
		uint32* vao;
		uint32* vbo;
		uint32* ebo;
		uint32* elementCount;
		uint32* textureID;		
	};

	RenderComponentData renderComponentData;

public:
	~OpenGLSystem();
	OpenGLSystem();

	bool InitializeSystem(int32 inWidth, int32 inHeight);

	ShaderProgram* GetDefaultShader() const { return defaultShader; }

	void PreRender();
	void RenderUI();	
	void PostRender();

	virtual uint32 CreateComponent(const Entity& e, void* componentData) override;
	virtual void Render() override;

	bool GetComponent(const Entity& e, uint32& outComponent) const;

	void SetComponentMesh(const std::string& meshPath, const Entity& e);

private:
	void AllocateMemory(int32 size);

	void LoadMeshData(uint32 componentIndex, const DrawData& tempData);

	void HandleWindowResized();
	void SetupUI();

	SDL_Window* window = nullptr;
	SDL_GLContext glContext = nullptr;

	int32 width = 0;
	int32 height = 0;

	CameraSystem* defaultCamera = nullptr;
	ShaderProgram* defaultShader = nullptr;

	std::unordered_map<Entity, uint32> instances;
};

REGISTER_SYSTEM(OpenGLSystem)
REGISTER_CREATION_FUNCTION(OpenGLSystem, ComponentType::RenderComponent)