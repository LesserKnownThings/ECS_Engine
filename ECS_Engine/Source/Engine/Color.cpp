#include "Color.h"

namespace LKT
{
	const Color Color::red = Color(1.f, 0.f, 0.f);
	const Color Color::blue = Color(0.0f, 0.0f, 1.0f);
	const Color Color::green = Color(0.0f, 1.0f, 0.0f);
	const Color Color::yellow = Color(1.0f, 1.0f, 0.0f);
	const Color Color::white = Color(1.0f, 1.0f, 1.0f);
	const Color Color::black = Color(0.0f, 0.0f, 0.0f);

	glm::vec3 Color::GetVector() const
	{
		return glm::vec3(r, g, b);
	}

	glm::vec4 Color::GetVectorAlpha() const
	{
		return glm::vec4(r, g, b, a);
	}
}