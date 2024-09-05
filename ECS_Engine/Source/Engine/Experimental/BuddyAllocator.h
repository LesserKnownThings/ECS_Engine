#pragma once

#include <cstdint>

constexpr int32_t MAX_LEVELS = 32;

/// <summary>
/// The reason this is experimental is I'm not sure if I'm going to use this. The plan is to have a hybrid system where the buddy allocator can handle the entities and their components since they're pretty small and I can use malloc for resource management like textures, meshes, etc.
/// 
/// This system is not used yet, I'm still thinking if I should use it or not.
/// </summary>

class BuddyAllocator
{
public:
	~BuddyAllocator();
	BuddyAllocator(uint64_t size);

	void* Allocate(uint64_t size);
	void Deallocate(void* ptr, uint64_t size);

private:
	void* GetBuddy(void* ptr, uint64_t size);
	void RemoveFromFreeList(void* ptr, int32_t level);

	bool IsFree(void* buddy, int32_t level) const;

	void* freeList[MAX_LEVELS];
	char* memory;
	uint64_t totalSize;
};