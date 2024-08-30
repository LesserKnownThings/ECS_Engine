#pragma once

#include <cstdlib>
#include <random>

namespace LKT
{
	namespace Math
	{
		static float RandomRange(float min, float max)
		{
			std::random_device rd;
			std::mt19937 gen(rd());

			std::uniform_real_distribution <> dis(min, max);

			return dis(gen);
		}

		static float RandomValue()
		{
			std::random_device rd;
			std::mt19937 gen(rd());

			std::uniform_real_distribution <float> dis(0.0f, 1.0f);
			return dis(gen);
		}
	}
}