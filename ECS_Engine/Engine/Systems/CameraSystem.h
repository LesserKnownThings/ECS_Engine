#pragma once

#include "../Math/Numerals.h"
#include "../Math/Transforms.h"

class ShaderProgram;

enum class CameraType
{
	Orthographic,
	Perspective
};

class CameraSystem
{
public:
	~CameraSystem() = default;
	CameraSystem(ShaderProgram* inShaderProgram, int32 inWidth, int32 inHeight);

	void ChangeType(CameraType inType, ShaderProgram* shaderProgram);

	void SetFieldOfView(float inFieldOfView, ShaderProgram* shaderProgram);
	void SetNearView(float inNearView, ShaderProgram* shaderProgram);
	void SetFarView(float inFarView, ShaderProgram* shaderProgram);

	void SetOrthographicSize(float inOrthographicSize, ShaderProgram* shaderProgram);
	void SetCameraZ(float inCameraZ, ShaderProgram* shaderProgram);

	void SetPosition(const Vector3& newPosition, ShaderProgram* shaderProgram);
	void SetRotation(const Vector3& eulerAngles, ShaderProgram* shaderProgram);

	void Move(const Vector3& direction, ShaderProgram* shaderProgram);
	void Rotate(const Vector3& axis, ShaderProgram* shaderProgram);

private:
	void HandleTypeChanged(ShaderProgram* shaderProgram);
	void SetPerspectiveCamera();
	void SetOrthographicCamera();
	void UpdateVectors(ShaderProgram* shaderProgram);

	CameraType cameraType = CameraType::Perspective;

	//***Perspective settings
	float fieldOfView = 45.0f;
	float nearView = 5.0f;
	float farView = 6000.0f;

	Vector3 forward = Vector3(0.0f, 0.0f, 1.0f);
	Vector3 right = Vector3(1.0f, 0.0f, 0.0f);
	Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 worldUp = Vector3(0.0f, 1.0f, 0.0f);
	
	//***Orthographic settings
	float ortographicSize = 2.0f;
	float cameraZ = 1.0f;

	//***Common settings
	int32 width = 0;
	int32 height = 0;

	Vector3 position = Vector3(0.0f, 0.0, -100.0f);
	
	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;

	Mat4 projection = Mat4(1.0f);
	Mat4 view = Mat4(1.0f);
};