#include "CameraSystem.h"
#include "glew/glew.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "SDL/SDL.h"
#include "Systems/InputSystem/InputManagerSystem.h"
#include "Systems/ShaderSystem/ShaderManager.h"
#include "Systems/ShaderSystem/ShaderProgram.h"
#include "Systems/TaskManagerSystem.h"

#include <iostream>

namespace LKT
{
    CameraSystem::CameraSystem(int32_t inWidth, int32_t inHeight)
        : width(inWidth), height(inHeight)
    {
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // This is set in the shader program uniform buffer settings
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 2 * sizeof(glm::mat4));

        HandleTypeChanged();
        UpdateVectors();
        SetRotation(glm::vec3(pitch, yaw, 0.0f));

        TaskManagerSystem::Get().RegisterTask(this, &CameraSystem::Process);

        InputManagerSystem::Get().onMousePressed.Bind(this, &CameraSystem::HandleMousePressed);
        InputManagerSystem::Get().onMouseReleased.Bind(this, &CameraSystem::HandleMouseReleased);
    }

    void CameraSystem::Uninitialize()
    {
        TaskManagerSystem::Get().RemoveTask(this);
        glDeleteBuffers(1, &ubo);
    }

    void CameraSystem::ChangeType(ECameraType inType)
    {
        cameraType = inType;

        HandleTypeChanged();
    }

    void CameraSystem::HandleWindowResize(float inWidth, float inHeight)
    {
        width = inWidth;
        height = inHeight;

        HandleTypeChanged();
    }

    void CameraSystem::SetFieldOfView(float inFieldOfView)
    {
        if (cameraType == ECameraType::Perspective)
        {
            fieldOfView = inFieldOfView;
            SetPerspectiveCamera();
            UpdateProjection();
        }
    }

    void CameraSystem::SetNearView(float inNearView)
    {
        if (cameraType == ECameraType::Perspective)
        {
            nearView = inNearView;
            SetPerspectiveCamera();
            UpdateProjection();
        }
    }

    void CameraSystem::SetFarView(float inFarView)
    {
        if (cameraType == ECameraType::Perspective)
        {
            farView = inFarView;
            SetPerspectiveCamera();
            UpdateProjection();
        }
    }

    void CameraSystem::SetOrthographicSize(float inOrthographicSize)
    {
        if (cameraType == ECameraType::Orthographic)
        {
            ortographicSize = inOrthographicSize;
            SetOrthographicCamera();
            UpdateProjection();
        }
    }

    void CameraSystem::SetCameraZ(float inCameraZ)
    {
        if (cameraType == ECameraType::Orthographic)
        {
            cameraZ = inCameraZ;
            SetOrthographicCamera();
            UpdateProjection();
        }
    }

    void CameraSystem::SetPosition(const glm::vec3 &newPosition)
    {
        position = newPosition;
        UpdateVectors();
    }

    void CameraSystem::SetRotation(const glm::vec3 &eulerAngles)
    {
        glm::vec3 correctedEulers = eulerAngles;

        if (correctedEulers.y > 360.0f || correctedEulers.y < -360.0f)
        {
            correctedEulers.y = 0.0f;
        }

        correctedEulers.x = glm::clamp(correctedEulers.x, -89.9f, 89.9f);

        yaw = correctedEulers.y;
        pitch = correctedEulers.x;

        glm::vec3 front = glm::vec3(0.0f);

        front.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));

        forward = glm::normalize(front);

        right = glm::normalize(glm::cross(forward, worldUp));
        up = glm::normalize(glm::cross(right, forward));

        UpdateVectors();
    }

    void CameraSystem::Move(const glm::vec3 &direction)
    {
        position += direction;
        UpdateVectors();
    }

    void CameraSystem::Rotate(const glm::vec3 &axis)
    {
        const glm::vec3 eulers(axis.x + pitch, yaw + axis.y, axis.z);
        SetRotation(eulers);
    }

    void CameraSystem::HandleTypeChanged()
    {
        switch (cameraType)
        {
        case ECameraType::Orthographic:
            SetOrthographicCamera();
            break;

        default:
            SetPerspectiveCamera();
            break;
        }

        UpdateProjection();
    }

    void CameraSystem::SetPerspectiveCamera()
    {
        if (width > 0 && height > 0)
        {
            projection = glm::perspective(glm::radians(fieldOfView), static_cast<float>(width) / static_cast<float>(height), nearView, farView);
        }
    }

    void CameraSystem::SetOrthographicCamera()
    {
        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

        float left = -ortographicSize * aspectRatio;
        float right = ortographicSize * aspectRatio;

        projection = glm::ortho(left, right, -ortographicSize, ortographicSize, -cameraZ, cameraZ);
    }

    void CameraSystem::UpdateVectors()
    {
        if (cameraType == ECameraType::Perspective)
        {
            view = glm::lookAt(
                position,
                position + forward,
                up);
        }
        else
        {
            glm::vec3 pos = position;
            pos.y *= -1; // SDL y is inverted
            view = glm::translate(glm::mat4(1.0f), pos);
        }

        UpdateView();
    }

    void CameraSystem::UpdateProjection()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void CameraSystem::UpdateView()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void CameraSystem::HandleMousePressed(const SDL_MouseButtonEvent &mouseEvent)
    {
        if (SDL_Window *currentWindow = SDL_GL_GetCurrentWindow())
        {
            // SDL_SetWindowRelativeMouseMode(currentWindow, SDL_TRUE);
        }

        if (mouseEvent.button == SDL_BUTTON_RIGHT)
        {
            movementType = MOUSE_LOOK;
        }
        else if (mouseEvent.button == SDL_BUTTON_MIDDLE)
        {
            movementType = MOUSE_MOVE;
        }
    }

    void CameraSystem::HandleMouseReleased(const SDL_MouseButtonEvent &mouseEvent)
    {
        if (mouseEvent.button == SDL_BUTTON_RIGHT)
        {
            movementType &= ~MOUSE_LOOK;
        }
        else if (mouseEvent.button == SDL_BUTTON_MIDDLE)
        {
            movementType &= ~MOUSE_MOVE;
        }

        if (movementType == 0)
        {
            if (SDL_Window *currentWindow = SDL_GL_GetCurrentWindow())
            {
                //  SDL_SetWindowRelativeMouseMode(currentWindow, SDL_FALSE);
            }
        }
    }

    // TODO this should not affect a game, it should only affect the engine since a user might want a custom movement in their game or no movement at all
    void CameraSystem::Process(float deltaTime)
    {
        glm::vec2 mouseDelta = InputManagerSystem::Get().GetMouseDelta();
        glm::vec3 movementVec = glm::vec3(0.0f);

        if (movementType & MOUSE_LOOK)
        {
            movementVec = InputManagerSystem::Get().GetHorizontalAxis() * right + InputManagerSystem::Get().GetVerticalAxis() * forward;
            mouseDelta *= deltaTime * rotationSpeed;

            Rotate(glm::vec3(-mouseDelta.y, mouseDelta.x, 0.0f));
        }
        else if (movementType & MOUSE_MOVE)
        {
            if (cameraType == Perspective)
            {
                movementVec = mouseDelta.x * right + -mouseDelta.y * up;
            }
            else
            {
                movementVec = mouseDelta.x * -glm::vec3(1.0f, 0.0f, 0.0f) + -mouseDelta.y * glm::vec3(0.0f, 1.0f, 0.0f);
            }
        }

        Move(movementVec * deltaTime * movementSpeed);
    }
}