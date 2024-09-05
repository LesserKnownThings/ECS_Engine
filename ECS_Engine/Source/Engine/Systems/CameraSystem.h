#pragma once

#include "glm/glm.hpp"

struct SDL_MouseButtonEvent;

namespace LKT
{
	constexpr uint8_t MOUSE_MOVE = 0x01;
	constexpr uint8_t MOUSE_LOOK = 0x02;

	class ShaderProgram;

	enum class EMovementType : uint8_t
	{
		Mouse_Move,
		Mouse_Look
	};

	enum ECameraType : uint8_t
	{
		Orthographic,
		Perspective
	};

	class CameraSystem
	{
	public:
		~CameraSystem() = default;
		CameraSystem(int32_t inWidth, int32_t inHeight);

		void Uninitialize();

		void ChangeType(ECameraType inType);
		void HandleWindowResize(float inWidth, float inHeight);

		void SetFieldOfView(float inFieldOfView);
		void SetNearView(float inNearView);
		void SetFarView(float inFarView);

		void SetOrthographicSize(float inOrthographicSize);
		void SetCameraZ(float inCameraZ);

		void SetPosition(const glm::vec3& newPosition);
		void SetRotation(const glm::vec3& eulerAngles);

		void Move(const glm::vec3& direction);
		void Rotate(const glm::vec3& axis);

		const glm::vec3& GetRightVector() const { return right; }

	private:
		void HandleTypeChanged();
		void SetPerspectiveCamera();
		void SetOrthographicCamera();
		void UpdateVectors();

		void UpdateProjection();
		void UpdateView();

		void HandleMousePressed(const SDL_MouseButtonEvent& mouseEvent);
		void HandleMouseReleased(const SDL_MouseButtonEvent& mouseEvent);
		void Process(float deltaTime);

		ECameraType cameraType = Perspective;	

		//***Perspective settings
		float fieldOfView = 45.0f;
		float nearView = 5.0f;
		float farView = 6000.0f;

		glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

		//***Orthographic settings
		float ortographicSize = 10.0f;
		float cameraZ = 1000.0f;

		//***Common settings
		int32_t width = 0;
		int32_t height = 0;

		glm::vec3 position = glm::vec3(0.0f, 0.0, 50.0f);

		float pitch = 0.0f;
		float yaw = 0.0f;
		float roll = 0.0f;

		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);

		uint32_t ubo;

		uint8_t movementType = 0;

		float movementSpeed = 15.0f;
		float rotationSpeed = 10.0f;
	};
}