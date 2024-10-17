#pragma once

#include <random>
#include <thread>

namespace LKT
{
	namespace Random
	{
		static thread_local std::mt19937 gen;

		static void Seed()
		{
			std::random_device rd;
			gen.seed(rd() ^ (std::chrono::steady_clock::now().time_since_epoch().count() +
							 std::hash<std::thread::id>()(std::this_thread::get_id())));
		}

		template <typename T>
		static T RandomRange(T from, T to)
		{
			if constexpr (std::is_integral<T>::value)
			{
				std::uniform_int_distribution<T> dis(from, to);
				return dis(gen);
			}
			else if constexpr (std::is_floating_point<T>::value)
			{
				std::uniform_real_distribution<T> dis(from, to);
				return dis(gen);
			}
			else
			{
				throw std::invalid_argument("Unsupported type for random range");
			}
		}

		static float Value()
		{
			return RandomRange<float>(0.0f, 1.0f);
		}
	}
}