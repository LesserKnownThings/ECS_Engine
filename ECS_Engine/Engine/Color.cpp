#include "Color.h"

const Color Color::red = Color(1.f, 0.f, 0.f);
const Color Color::blue = Color(0.0f, 0.0f, 1.0f);
const Color Color::green = Color(0.0f, 1.0f, 0.0f);
const Color Color::yellow = Color(1.0f, 1.0f, 0.0f);
const Color Color::white = Color(1.0f, 1.0f, 1.0f);
const Color Color::black = Color(0.0f, 0.0f, 0.0f);

Vector3 Color::GetVector() const
{
	return Vector3(r, g, b);
}