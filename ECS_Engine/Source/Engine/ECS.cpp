#include "Engine.h"

#include <cstdint>
#include <iostream>

int32_t main(int32_t argc, char *args[])
{

	for (int32_t i = 0; i < argc; ++i)
	{
		std::cout << args[i] << std::endl;
	}

	LKT::Engine e;
	return 1;
}