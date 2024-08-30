#pragma once

#include <random>
namespace LKT
{
	namespace Random
	{
		template<typename T>
		static T RandomRange(T from, T to)
		{
			std::random_device rd;
			std::mt19937 gen(rd());

			std::uniform_real_distribution<> dis(from, to);

			return dis(gen);
		}

		static float Value()
		{
			return RandomRange<float>(0.0f, 1.0f);
		}
	}
}