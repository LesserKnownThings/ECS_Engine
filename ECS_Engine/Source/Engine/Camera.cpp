#include "Camera.h"
#include "glew/glew.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "Systems/ShaderSystem/ShaderManager.h"
#include "Systems/ShaderSystem/ShaderProgram.h"

#include <iostream>

namespace LKT
{
    Camera::Camera(int32_t inWidth, int32_t inHeight, const glm::vec3 &initialPosition)
        : width(inWidth), height(inHeight), position(initialPosition)
    {
        ShaderManager::GetUBO(MATRICES_UBO_INDEX, ubo);
        // SetRotation(glm::vec3(pitch, yaw, 0.0f));
        lookDirection = position + forward;
    }

    void Camera::ChangeType(ECameraType inType)
    {
        cameraType = inType;
    }

    void Camera::ResizeWindow(int32_t inWidth, int32_t inHeight)
    {
        width = inWidth;
        height = inHeight;
    }

    void Camera::SetFieldOfView(float inFieldOfView)
    {
        if (cameraType == ECameraType::Perspective)
        {
            fieldOfView = inFieldOfView;
        }
    }

    void Camera::SetNearView(float inNearView)
    {
        if (cameraType == ECameraType::Perspective)
        {
            nearView = inNearView;
        }
    }

    void Camera::SetFarView(float inFarView)
    {
        if (cameraType == ECameraType::Perspective)
        {
            farView = inFarView;
        }
    }

    void Camera::SetOrthographicSize(float inOrthographicSize)
    {
        if (cameraType == ECameraType::Orthographic)
        {
            ortographicSize = inOrthographicSize;
        }
    }

    void Camera::SetCameraZ(float inCameraZ)
    {
        if (cameraType == ECameraType::Orthographic)
        {
            cameraZ = inCameraZ;
        }
    }

    void Camera::SetPosition(const glm::vec3 &newPosition)
    {
        position = newPosition;
    }

    void Camera::SetRotation(const glm::vec3 &eulerAngles)
    {
        rotation = glm::qua(glm::radians(eulerAngles));
        rotation = glm::normalize(rotation);

        UpdateDirections();
    }

    void Camera::UpdateDirections()
    {
        forward = glm::normalize(rotation * glm::vec3(0.f, 0.f, -1.f));
        const glm::vec3 worldUp = glm::vec3(0.f, 1.f, 0.f);
        right = glm::normalize(glm::cross(forward, worldUp));
        up = glm::cross(right, forward);
    }

    void Camera::Zoom(float amount)
    {
        position += forward * amount;
    }

    void Camera::Move(const glm::vec3 &direction)
    {
        position += direction;
    }

    void Camera::Rotate(glm::vec3 axis)
    {
        if (axis.x > 0.f || axis.x < 0.f || axis.y > 0.f || axis.y < 0.f)
        {
            const float deltaX = glm::radians(axis.y);
            const float deltaY = glm::radians(axis.x);

            glm::vec3 rotateForward = glm::rotate(forward, deltaY, right);
            rotateForward = glm::rotate(rotateForward, deltaX, up);
            rotateForward = glm::normalize(rotateForward);

            rotation = glm::quatLookAt(rotateForward, up);

            UpdateDirections();
        }
    }

    void Camera::RotateAround(const glm::vec3 &pos, glm::vec3 axis, float angle)
    {
        glm::vec3 offset = position - pos;

        // Making sure the user doesn't send weird stuff here
        axis = glm::normalize(axis);
        axis.z = 0.0f;

        glm::vec3 rotatedOffset = glm::rotate(offset, glm::radians(angle), axis);

        position = pos + rotatedOffset;

        const glm::vec3 tempForward = glm::normalize(pos - position);
        rotation = glm::quatLookAt(tempForward, up);

        UpdateDirections();
    }

    void Camera::Present()
    {
        UpdateProjectionType();
        UpdateVectors();
    }

    void Camera::UpdateProjectionType()
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

    void Camera::SetPerspectiveCamera()
    {
        if (width > 0 && height > 0)
        {
            projection = glm::perspective(glm::radians(fieldOfView), static_cast<float>(width) / static_cast<float>(height), nearView, farView);
        }
    }

    void Camera::SetOrthographicCamera()
    {
        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

        float left = -ortographicSize * aspectRatio;
        float right = ortographicSize * aspectRatio;

        projection = glm::ortho(left, right, -ortographicSize, ortographicSize, -cameraZ, cameraZ);
    }

    void Camera::UpdateVectors()
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

    void Camera::UpdateProjection()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void Camera::UpdateView()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    // void Camera::HandleMousePressed(const SDL_MouseButtonEvent &mouseEvent)
    // {
    //     if (SDL_Window *currentWindow = SDL_GL_GetCurrentWindow())
    //     {
    //         // SDL_SetWindowRelativeMouseMode(currentWindow, SDL_TRUE);
    //     }

    //     if (mouseEvent.button == SDL_BUTTON_RIGHT)
    //     {
    //         movementType = MOUSE_LOOK;
    //     }
    //     else if (mouseEvent.button == SDL_BUTTON_MIDDLE)
    //     {
    //         movementType = MOUSE_MOVE;
    //     }
    // }

    // void Camera::HandleMouseReleased(const SDL_MouseButtonEvent &mouseEvent)
    // {
    //     if (mouseEvent.button == SDL_BUTTON_RIGHT)
    //     {
    //         movementType &= ~MOUSE_LOOK;
    //     }
    //     else if (mouseEvent.button == SDL_BUTTON_MIDDLE)
    //     {
    //         movementType &= ~MOUSE_MOVE;
    //     }

    //     if (movementType == 0)
    //     {
    //         if (SDL_Window *currentWindow = SDL_GL_GetCurrentWindow())
    //         {
    //             //  SDL_SetWindowRelativeMouseMode(currentWindow, SDL_FALSE);
    //         }
    //     }
    // }

    // // TODO this should not affect a game, it should only affect the engine since a user might want a custom movement in their game or no movement at all
    // void Camera::Process(float deltaTime)
    // {
    //     glm::vec2 mouseDelta = InputManagerSystem::Get().GetMouseDelta();
    //     glm::vec3 movementVec = glm::vec3(0.0f);

    //     if (movementType & MOUSE_LOOK)
    //     {
    //         movementVec = InputManagerSystem::Get().GetHorizontalAxis() * right + InputManagerSystem::Get().GetVerticalAxis() * forward;
    //         mouseDelta *= deltaTime * rotationSpeed;

    //         Rotate(glm::vec3(-mouseDelta.y, mouseDelta.x, 0.0f));
    //     }
    //     else if (movementType & MOUSE_MOVE)
    //     {
    //         if (cameraType == Perspective)
    //         {
    //             movementVec = mouseDelta.x * right + -mouseDelta.y * up;
    //         }
    //         else
    //         {
    //             movementVec = mouseDelta.x * -glm::vec3(1.0f, 0.0f, 0.0f) + -mouseDelta.y * glm::vec3(0.0f, 1.0f, 0.0f);
    //         }
    //     }

    //     Move(movementVec * deltaTime * movementSpeed);
    // }
}