#pragma once

#include "Math/Numerals.h"
#include "Math/Transforms.h"
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
		CameraSystem(int32 inWidth, int32 inHeight);

		void Uninitialize();

		void ChangeType(ECameraType inType);
		void HandleWindowResize(float inWidth, float inHeight);

		void SetFieldOfView(float inFieldOfView);
		void SetNearView(float inNearView);
		void SetFarView(float inFarView);

		void SetOrthographicSize(float inOrthographicSize);
		void SetCameraZ(float inCameraZ);

		void SetPosition(const Vector3& newPosition);
		void SetRotation(const Vector3& eulerAngles);

		void Move(const Vector3& direction);
		void Rotate(const Vector3& axis);

		const Vector3& GetRightVector() const { return right; }

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

		Vector3 forward = Vector3(0.0f, 0.0f, -1.0f);
		Vector3 right = Vector3(1.0f, 0.0f, 0.0f);
		Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
		Vector3 worldUp = Vector3(0.0f, 1.0f, 0.0f);

		//***Orthographic settings
		float ortographicSize = 10.0f;
		float cameraZ = 1000.0f;

		//***Common settings
		int32 width = 0;
		int32 height = 0;

		Vector3 position = Vector3(0.0f, 0.0, 50.0f);

		float pitch = 0.0f;
		float yaw = 0.0f;
		float roll = 0.0f;

		Mat4 projection = Mat4(1.0f);
		Mat4 view = Mat4(1.0f);

		uint32_t ubo;

		uint8_t movementType = 0;

		float movementSpeed = 50.0f;
		float rotationSpeed = 10.0f;
	};
}