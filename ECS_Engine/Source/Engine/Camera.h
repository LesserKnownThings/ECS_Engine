#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include <cstdint>

namespace LKT
{
	class ShaderProgram;

	enum ECameraType : uint8_t
	{
		Orthographic,
		Perspective
	};

	class Camera
	{
	public:
		Camera(int32_t inWidth, int32_t inHeight, const glm::vec3 &initialPosition = glm::vec3(0.0f, 0.0f, 10.0f));

		void ChangeType(ECameraType inType);
		void ResizeWindow(int32_t inWidth, int32_t inHeight);

		void SetFieldOfView(float inFieldOfView);
		void SetNearView(float inNearView);
		void SetFarView(float inFarView);

		void SetOrthographicSize(float inOrthographicSize);
		void SetCameraZ(float inCameraZ);

		void SetPosition(const glm::vec3 &newPosition);
		void SetRotation(const glm::vec3 &eulerAngles);

		void Zoom(float amount);
		void Move(const glm::vec3 &direction);
		void Rotate(const glm::vec3 &axis);
		void RotateAround(const glm::vec3 &pos, glm::vec3 axis, float angle);

		void Present();

		const glm::vec3 &GetRightVector() const { return right; }
		const glm::mat4 &GetView() const { return view; }
		const glm::mat4 &GetProjection() const { return projection; }

	private:
		void SetPerspectiveCamera();
		void SetOrthographicCamera();

		void UpdateVectors();
		void UpdateProjectionType();
		void UpdateProjection();
		void UpdateView();
		void UpdateDirections();

		ECameraType cameraType = Perspective;

		//***Perspective settings
		float fieldOfView = 45.0f;
		float nearView = 1.0f;
		float farView = 6000.0f;

		glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::vec3 lookDirection = glm::vec3(0.0f);

		//***Orthographic settings
		float ortographicSize = 10.0f;
		float cameraZ = 1000.0f;

		//***Common settings
		int32_t width = 0;
		int32_t height = 0;

		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::quat rotation = glm::vec3(0.f);

		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);

		uint32_t ubo;

		uint8_t movementType = 0;

		float movementSpeed = 15.0f;
		float rotationSpeed = 10.0f;
	};
}