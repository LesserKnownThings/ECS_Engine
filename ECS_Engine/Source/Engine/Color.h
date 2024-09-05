#pragma once

#include "glm/glm.hpp"

#include <cstdint>

namespace LKT
{
	struct Color
	{
	public:
		Color() {}
		Color(float inR, float inG, float inB)
			: r(inR), g(inG), b(inB), a(1.f) {}
		Color(float inR, float inG, float inB, float inA)
			: r(inR), g(inG), b(inB), a(inA) {}

		glm::vec3 GetVector() const;
		glm::vec4 GetVectorAlpha() const;

		static const Color red;
		static const Color blue;
		static const Color green;
		static const Color yellow;
		static const Color white;
		static const Color black;

		float r = 0;
		float g = 0;
		float b = 0;
		float a = 1;
	};
}