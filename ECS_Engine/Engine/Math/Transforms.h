#pragma once

#include "glm.hpp"
#include "gtc/type_ptr.hpp"

#define Vector2 glm::vec2
#define Vector3 glm::vec3
#define Quaternion glm::quat
#define Mat4 glm::mat4

#define DATA(inValue) (glm::value_ptr(inValue))
#define TO_RADIANS(inValue) (glm::radians(inValue))