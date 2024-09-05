#pragma once

#include "Object.h"
#include "glm/glm.hpp"

namespace LKT
{
	struct InputActionData
	{
		int32_t key = -1;
		glm::vec3 value = glm::vec3(0.0f);
	};

	class InputMapping : public Object
	{

	};
}