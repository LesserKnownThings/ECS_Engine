#include "ResourceManager.h"

ResourceManager::~ResourceManager()
{

}

ResourceManager& ResourceManager::Get()
{
	static ResourceManager instance;
	return instance;
}

ResourceManager::ResourceManager()
{

}