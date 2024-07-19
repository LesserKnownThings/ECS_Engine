#include "OpenGLSystem.h"
#include "assert.h"
#include "glew.h"
#include "InputSystem.h"
#include "MeshLoadingSystem.h"
#include "SDL.h"

#include <iostream>

namespace
{
	static void CheckSDLError(int32_t line)
	{
		const char* error = SDL_GetError();
		if (*error != '\0')
		{
			printf("SDL Error: %s\n", error);
			if (line != -1)
				printf(" + line: %i\n", line);
			SDL_ClearError();
		}
	}
}

OpenGLSystem::~OpenGLSystem()
{
	if (glContext != nullptr)
	{
		SDL_GL_DeleteContext(glContext);
		glContext = nullptr;
	}

	if (window != nullptr)
	{
		SDL_DestroyWindow(window);
		window = nullptr;
	}
}

OpenGLSystem::OpenGLSystem()
{
	AllocateMemory(DEFAULT_ALLOCATED_MEMORY);

	InputSystem::Get().onWindowResized.Bind(this, &OpenGLSystem::HandleWindowResized);
}

bool OpenGLSystem::InitializeSystem(int32 inWidth, int32 inHeight)
{
	width = inWidth;
	height = inHeight;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);

	window = SDL_CreateWindow("ECS Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	::CheckSDLError(__LINE__);

	glContext = SDL_GL_CreateContext(window);
	::CheckSDLError(__LINE__);

	if (SDL_GL_MakeCurrent(window, glContext) != 0)
	{
		std::cerr << "Failed to make OpenGL context current: " << SDL_GetError() << std::endl;
		return false;
	}

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (err != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
		return false;
	}

	if (SDL_GL_SetSwapInterval(1) != 0)
	{
		printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEBUG_OUTPUT);

	glViewport(0, 0, width, height);
	glScissor(0, 0, width, height);

	defaultShader.InitializeShader();

	return true;
}

void OpenGLSystem::PreRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLSystem::Render()
{
	defaultShader.Use();

	for (uint32 i = 0; i < renderComponentData.instancesCount; ++i)
	{
		glBindVertexArray(renderComponentData.vao[i]);
		glDrawElements(GL_TRIANGLES, renderComponentData.elementCount[i], GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
}

void OpenGLSystem::PostRender()
{
	SDL_GL_SwapWindow(window);
}

uint32 OpenGLSystem::CreateComponent(const Entity& e)
{
	uint32 size = static_cast<uint32>(instances.size());

	if (renderComponentData.allocatedInstances <= size)
	{
		AllocateMemory(renderComponentData.allocatedInstances * 2);
	}

	instances.emplace(e, size);
	renderComponentData.instancesCount++;

	renderComponentData.entity[size] = e;
	renderComponentData.vao[size] = 0;
	renderComponentData.vbo[size] = 0;
	renderComponentData.ebo[size] = 0;
	renderComponentData.textureID[size] = 0;
	renderComponentData.elementCount[size] = 0;

	return size;
}

bool OpenGLSystem::GetComponent(const Entity& e, uint32& outComponent) const
{
	const auto it = instances.find(e);

	if (it != instances.end())
	{
		outComponent = it->second;
		return true;
	}

	return false;
}

void OpenGLSystem::SetComponentMesh(const std::string& meshPath, const Entity& e)
{
	const auto it = instances.find(e);

	if (it != instances.end())
	{
		const uint32 componentIndex = it->second;

		if (!MeshLoadingSystem::Get().GetMeshData(meshPath, [this, componentIndex](const DrawData& tempData)
			{
				LoadMeshData(componentIndex, tempData);
			}))
		{
			printf("Error, could not load mesh %s", meshPath.data());
		}
	}
}

void OpenGLSystem::AllocateMemory(int32 size)
{
	assert(size > renderComponentData.allocatedInstances);

	RenderComponentData newData;
	const int32 sizeToAllocate = size * (sizeof(uint32) * 6);

	newData.buffer = malloc(sizeToAllocate);

	newData.instancesCount = renderComponentData.instancesCount;
	newData.allocatedInstances = size;

	newData.entity = (Entity*)(newData.buffer);
	newData.vao = (uint32*)(size + newData.entity);
	newData.vbo = size + newData.vao;
	newData.ebo = size + newData.vbo;
	newData.elementCount = size + newData.ebo;
	newData.textureID = size + newData.elementCount;

	memcpy(newData.entity, renderComponentData.entity, renderComponentData.instancesCount * sizeof(Entity));
	memcpy(newData.vao, renderComponentData.vao, renderComponentData.instancesCount * sizeof(uint32));
	memcpy(newData.vbo, renderComponentData.vbo, renderComponentData.instancesCount * sizeof(uint32));
	memcpy(newData.ebo, renderComponentData.ebo, renderComponentData.instancesCount * sizeof(uint32));
	memcpy(newData.elementCount, renderComponentData.elementCount, renderComponentData.instancesCount * sizeof(uint32));
	memcpy(newData.textureID, renderComponentData.textureID, renderComponentData.instancesCount * sizeof(uint32));

	free(renderComponentData.buffer);
	renderComponentData = newData;
}

void OpenGLSystem::LoadMeshData(uint32 componentIndex, const DrawData& tempData)
{
	renderComponentData.elementCount[componentIndex] = tempData.indicesCount;

	glGenVertexArrays(1, &renderComponentData.vao[componentIndex]);
	glBindVertexArray(renderComponentData.vao[componentIndex]);

	glGenBuffers(1, &renderComponentData.vbo[componentIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, renderComponentData.vbo[componentIndex]);

	glGenBuffers(1, &renderComponentData.ebo[componentIndex]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderComponentData.ebo[componentIndex]);

	const int32 vertexBufferSize = tempData.vertexCount * sizeof(VertexData);
	
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, tempData.vertexData.buffer, GL_STATIC_DRAW);

	const int32 elementBufferSize = tempData.indicesCount * sizeof(uint32);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementBufferSize, tempData.indices.data(), GL_STATIC_DRAW);

	uint32 offset = 0;

	//All the data is tightly packed so we can start at index 0

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
	glEnableVertexAttribArray(0);

	offset += sizeof(Vector3);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
	glEnableVertexAttribArray(1);

	offset += sizeof(Vector3);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void OpenGLSystem::HandleWindowResized()
{
	SDL_GetWindowSize(window, &width, &height);

	glViewport(0, 0, width, height);
	glScissor(0, 0, width, height);

	InputSystem::Get().onWindowResizedParams.Invoke(width, height);
}