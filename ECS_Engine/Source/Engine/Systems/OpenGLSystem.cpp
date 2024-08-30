#include "OpenGLSystem.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include "assert.h"
#include "CameraSystem.h"
#include "glew.h"
#include "InputSystem/InputManagerSystem.h"
#include "MeshLoadingSystem.h"
#include "ParticleSystem/ParticleSystem.h"
#include "SDL.h"
#include "ShaderSystem/ShaderProgram.h"
#include "ShaderSystem/ShaderManager.h"
#include "TaskManagerSystem.h"
#include "TransformSystem.h"

#include <iostream>

namespace LKT
{
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

	OpenGLSystem::OpenGLSystem()
	{
		AllocateMemory(DEFAULT_ALLOCATED_MEMORY);

		InputManagerSystem::Get().onWindowResized.Bind(this, &OpenGLSystem::HandleWindowResized);
	}

	OpenGLSystem& OpenGLSystem::Get()
	{
		static OpenGLSystem instance;
		return instance;
	}

	bool OpenGLSystem::InitializeSystem(int32_t inWidth, int32_t inHeight)
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
		LKT::CheckSDLError(__LINE__);

		glContext = SDL_GL_CreateContext(window);
		LKT::CheckSDLError(__LINE__);

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

		SetupUI();

		ShaderManager::Get().onPostCompile.Bind(this, &OpenGLSystem::HandlePostShaderCompile);

		return true;
	}

	void OpenGLSystem::Uninitialize()
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

		defaultCamera->Uninitialize();
		delete defaultCamera;
	}

	void OpenGLSystem::HandlePostShaderCompile()
	{
		ShaderManager::Get().ActivateShader(defaultShaderName);
		defaultCamera = new CameraSystem(width, height);
	}

	void OpenGLSystem::PreRender()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLSystem::PreRenderUI()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
	}

	void OpenGLSystem::Render()
	{
		PreRender();
		PreRenderUI();

		ShaderManager::Get().ActivateShader(defaultShaderName);

		for (const auto& kvp : instances)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, renderComponent.textureID[kvp.second]);

			glBindVertexArray(renderComponent.vao[kvp.second]);
			glDrawElements(GL_TRIANGLES, renderComponent.elementCount[kvp.second], GL_UNSIGNED_INT, 0);

			glm::mat4 model = glm::mat4(1.0f);
			TransformSystem::Get().GetModel(kvp.first, model);
			ShaderManager::Get().SetMat4f("model", model);

			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		}

		TaskManagerSystem::Get().ExecuteTasks(RENDER_HANDLE);

		PostRenderUI();
		PostRender();
	}

	void OpenGLSystem::PostRenderUI()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void OpenGLSystem::PostRender()
	{
		SDL_GL_SwapWindow(window);
	}

	uint32_t OpenGLSystem::CreateComponent(const Entity& e, void* componentData)
	{
		int32_t size = instances.size();

		if (renderComponent.allocatedInstances <= size)
		{
			AllocateMemory(renderComponent.allocatedInstances * 2);
		}

		instances.emplace(e, size);
		renderComponent.instancesCount++;

		renderComponent.entity[size] = e;
		renderComponent.vao[size] = 0;
		renderComponent.vbo[size] = 0;
		renderComponent.ebo[size] = 0;
		renderComponent.textureID[size] = 0;
		renderComponent.elementCount[size] = 0;

		return size;
	}

	void OpenGLSystem::CreateComponents(int32_t entityCount, Entity* entities, void* componentData)
	{
		const uint32_t startingIndex = instances.size();

		if (renderComponent.allocatedInstances < entityCount)
		{
			AllocateMemory(entityCount > startingIndex ? entityCount * 2 : startingIndex * 2);
		}

		renderComponent.instancesCount += entityCount;

		for (int32_t i = 0; i < entityCount; ++i)
		{
			instances.emplace(entities[i].id, startingIndex + i);
		}

		const uint32_t size = sizeof(uint32_t) * entityCount;

		uint32_t* modBuffer = static_cast<uint32_t*>(componentData);

		memcpy(&renderComponent.entity[startingIndex], entities, sizeof(Entity) * entityCount);

		memcpy(&renderComponent.vao[startingIndex], modBuffer, size);
		memcpy(&renderComponent.vbo[startingIndex], modBuffer + entityCount, size);
		memcpy(&renderComponent.ebo[startingIndex], modBuffer + entityCount * 2, size);
		memcpy(&renderComponent.elementCount[startingIndex], modBuffer + entityCount * 3, size);
		memcpy(&renderComponent.textureID[startingIndex], modBuffer + entityCount * 4, size);
	}

	bool OpenGLSystem::GetComponent(const Entity& e, uint32_t& outComponent) const
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
			const uint32_t componentIndex = it->second;

			if (!MeshLoadingSystem::Get().GetMeshData(meshPath, [this, componentIndex](const DrawData& tempData)
				{
					LoadMeshData(componentIndex, tempData);
				}))
			{
				printf("Error, could not load mesh %s", meshPath.data());
			}
		}
	}

	void OpenGLSystem::AllocateMemory(int32_t size)
	{
		assert(size > renderComponent.allocatedInstances);

		RenderComponent newComponent;
		const int32_t sizeToAllocate = size * (sizeof(uint32_t) * 6);

		newComponent.buffer = malloc(sizeToAllocate);

		newComponent.instancesCount = renderComponent.instancesCount;
		newComponent.allocatedInstances = size;

		newComponent.entity = (Entity*)(newComponent.buffer);
		newComponent.vao = (uint32_t*)(size + newComponent.entity);
		newComponent.vbo = size + newComponent.vao;
		newComponent.ebo = size + newComponent.vbo;
		newComponent.elementCount = size + newComponent.ebo;
		newComponent.textureID = size + newComponent.elementCount;

		memcpy(newComponent.entity, renderComponent.entity, renderComponent.instancesCount * sizeof(Entity));
		memcpy(newComponent.vao, renderComponent.vao, renderComponent.instancesCount * sizeof(uint32_t));
		memcpy(newComponent.vbo, renderComponent.vbo, renderComponent.instancesCount * sizeof(uint32_t));
		memcpy(newComponent.ebo, renderComponent.ebo, renderComponent.instancesCount * sizeof(uint32_t));
		memcpy(newComponent.elementCount, renderComponent.elementCount, renderComponent.instancesCount * sizeof(uint32_t));
		memcpy(newComponent.textureID, renderComponent.textureID, renderComponent.instancesCount * sizeof(uint32_t));

		free(renderComponent.buffer);
		renderComponent = newComponent;
	}

	void OpenGLSystem::LoadMeshData(uint32_t componentIndex, const DrawData& tempData)
	{
		renderComponent.elementCount[componentIndex] = tempData.indicesCount;

		glGenVertexArrays(1, &renderComponent.vao[componentIndex]);
		glBindVertexArray(renderComponent.vao[componentIndex]);

		glGenBuffers(1, &renderComponent.vbo[componentIndex]);
		glBindBuffer(GL_ARRAY_BUFFER, renderComponent.vbo[componentIndex]);

		glGenBuffers(1, &renderComponent.ebo[componentIndex]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderComponent.ebo[componentIndex]);

		const int32_t vertexBufferSize = tempData.vertexCount * sizeof(VertexData);

		glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, tempData.vertexData.buffer, GL_STATIC_DRAW);

		const int32_t elementBufferSize = tempData.indicesCount * sizeof(uint32_t);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementBufferSize, tempData.indices.data(), GL_STATIC_DRAW);

		uint32_t offset = 0;

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(uintptr_t)offset);		

		offset += sizeof(glm::vec3) * tempData.vertexCount;

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(uintptr_t)offset);		

		offset += sizeof(glm::vec3) * tempData.vertexCount;

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(uintptr_t)offset);		

		glBindVertexArray(0);
	}

	void OpenGLSystem::HandleWindowResized()
	{
		ShaderManager::Get().ActivateShader(defaultShaderName);

		SDL_GetWindowSize(window, &width, &height);

		glViewport(0, 0, width, height);
		glScissor(0, 0, width, height);

		defaultCamera->HandleWindowResize(width, height);

		InputManagerSystem::Get().onWindowResizedParams.Invoke(width, height);
	}

	void OpenGLSystem::SetupUI()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

		io.DeltaTime = 0.05f;

		// Setup Dear ImGui style
		//ImGui::StyleColorsClassic();
		ImGui::StyleColorsLight();

		const char* glsl_version = "#version 330 core";

		// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForOpenGL(window, glContext);
		ImGui_ImplOpenGL3_Init(glsl_version);
	}
}