#pragma once

class ResourceManager
{
public:
	~ResourceManager();

	static ResourceManager& Get();

private:
	ResourceManager();
};