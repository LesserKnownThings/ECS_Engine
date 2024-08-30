#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#define Vector2 glm::vec2
#define Vector3 glm::vec3
#define Vector4 glm::vec4
#define Quaternion glm::quat
#define Mat4 glm::mat4

#define DATA(inValue) (glm::value_ptr(inValue))
#define TO_RADIANS(inValue) (glm::radians(inValue))
#define NORMALIZE(inValue) (glm::normalize(inValue))
#define CROSS(val1, val2) (glm::cross(val1, val2))

namespace
{
	
}