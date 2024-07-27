#include "CameraSystem.h"
#include "../ShaderProgram.h"

CameraSystem::CameraSystem(ShaderProgram* inShaderProgram, int32 inWidth, int32 inHeight)
    : width(inWidth), height(inHeight)
{
    HandleTypeChanged(inShaderProgram);
    UpdateVectors(inShaderProgram);
    SetRotation(Vector3(0.0f), inShaderProgram);
}

void CameraSystem::ChangeType(CameraType inType, ShaderProgram* shaderProgram)
{
    cameraType = inType;
    
    HandleTypeChanged(shaderProgram);
}

void CameraSystem::SetFieldOfView(float inFieldOfView, ShaderProgram* shaderProgram)
{
    if (cameraType == CameraType::Perspective)
    {
        fieldOfView = inFieldOfView;
        SetPerspectiveCamera();
        shaderProgram->SetMat4f("projection", DATA(projection));
    }
}

void CameraSystem::SetNearView(float inNearView, ShaderProgram* shaderProgram)
{
    if (cameraType == CameraType::Perspective)
    {
        nearView = inNearView;
        SetPerspectiveCamera();
        shaderProgram->SetMat4f("projection", DATA(projection));
    }
}

void CameraSystem::SetFarView(float inFarView, ShaderProgram* shaderProgram)
{
    if (cameraType == CameraType::Perspective)
    {
        farView = inFarView;
        SetPerspectiveCamera();
        shaderProgram->SetMat4f("projection", DATA(projection));
    }
}

void CameraSystem::SetOrthographicSize(float inOrthographicSize, ShaderProgram* shaderProgram)
{
    if (cameraType == CameraType::Orthographic)
    {
        ortographicSize = inOrthographicSize;
        SetOrthographicCamera();
        shaderProgram->SetMat4f("projection", DATA(projection));
    }
}

void CameraSystem::SetCameraZ(float inCameraZ, ShaderProgram* shaderProgram)
{
    if (cameraType == CameraType::Orthographic)
    {
        cameraZ = inCameraZ;
        SetOrthographicCamera();
        shaderProgram->SetMat4f("projection", DATA(projection));
    }
}

void CameraSystem::SetPosition(const Vector3& newPosition, ShaderProgram* shaderProgram)
{
    position = newPosition;
    UpdateVectors(shaderProgram);
}

void CameraSystem::SetRotation(const Vector3& eulerAngles, ShaderProgram* shaderProgram)
{
    glm::vec3 correctedEulers = eulerAngles;

    if (correctedEulers.x > 89.f)
    {
        correctedEulers.x = 89.f;
    }

    if (correctedEulers.x < -89.0f)
    {
        correctedEulers.x = -89.0f;
    }

    yaw = eulerAngles.y;
    pitch = eulerAngles.x;

    Vector3 front = Vector3(0.0f);
    front.x = std::sin(TO_RADIANS(yaw)) * std::cos(TO_RADIANS(pitch)); 
    front.y = std::sin(TO_RADIANS(pitch));  
    front.z = std::cos(TO_RADIANS(yaw))* std::cos(TO_RADIANS(pitch));

    forward = NORMALIZE(front);

    right = NORMALIZE(CROSS(forward, worldUp));
    up = NORMALIZE(CROSS(right, forward));

    UpdateVectors(shaderProgram);
}

void CameraSystem::Move(const Vector3& direction, ShaderProgram* shaderProgram)
{
    position += direction;
    UpdateVectors(shaderProgram);
}

void CameraSystem::Rotate(const Vector3& axis, ShaderProgram* shaderProgram)
{
    const Vector3 eulers(axis.x + pitch, yaw - axis.y, axis.z);
    SetRotation(eulers, shaderProgram);
}

void CameraSystem::HandleTypeChanged(ShaderProgram* shaderProgram)
{
    switch (cameraType)
    {
    case CameraType::Orthographic:
        SetOrthographicCamera();
        break;

    default:
        SetPerspectiveCamera();
        break;
    }

    shaderProgram->SetMat4f("projection", DATA(projection));
}

void CameraSystem::SetPerspectiveCamera()
{
    if (width > 0 && height > 0)
    {
        projection = glm::perspective(TO_RADIANS(fieldOfView), static_cast<float>(width) / static_cast<float>(height), nearView, farView);
    }
}

void CameraSystem::SetOrthographicCamera()
{
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    float left = -ortographicSize * aspectRatio;
    float right = ortographicSize * aspectRatio;

    projection = glm::ortho(left, right, -ortographicSize, ortographicSize, -cameraZ, cameraZ);
}

void CameraSystem::UpdateVectors(ShaderProgram* shaderProgram)
{
    if (cameraType == CameraType::Perspective)
    {
        view = glm::lookAt(
            position,
            position + forward,
            up
        );
    }
    else
    {
        Vector3 pos = position;
        pos.y *= -1; //SDL y is inverted
        view = glm::translate(Mat4(1.0f), pos);
    }

    shaderProgram->SetMat4f("view", DATA(view));
}
