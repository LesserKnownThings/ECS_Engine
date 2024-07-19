#pragma once

#include "../Entity.h"
#include "SDL_video.h"
#include "../ShaderProgram.h"

#include <string>
#include <unordered_map>

struct DrawData;

class OpenGLSystem
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

	ShaderProgram& GetDefaultShader() { return defaultShader; }

	void PreRender();
	void Render();
	void PostRender();

	uint32 CreateComponent(const Entity& e);
	bool GetComponent(const Entity& e, uint32& outComponent) const;

	void SetComponentMesh(const std::string& meshPath, const Entity& e);

private:
	void AllocateMemory(int32 size);

	void LoadMeshData(uint32 componentIndex, const DrawData& tempData);

	void HandleWindowResized();

	SDL_Window* window = nullptr;
	SDL_GLContext glContext = nullptr;

	int32 width = 0;
	int32 height = 0;

	ShaderProgram defaultShader;

	std::unordered_map<Entity, uint32> instances;
};