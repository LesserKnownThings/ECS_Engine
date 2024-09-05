#include "BuddyAllocator.h"

#include <cmath>
#include <cstring>
#include <string>

BuddyAllocator::~BuddyAllocator()
{
	delete[] memory;
}

BuddyAllocator::BuddyAllocator(uint64_t size)
{
	totalSize = static_cast<uint64_t>(1) << static_cast<uint64_t>(std::ceil(std::log2(size)));
	memory = new char[totalSize];
	std::memset(freeList, 0, sizeof(freeList));

	int32_t levels = std::log2(totalSize) + 1;
	freeList[levels - 1] = memory;
}

void* BuddyAllocator::Allocate(uint64_t size)
{
	uint64_t blockSize = static_cast<uint64_t>(1) << static_cast<uint64_t>(std::ceil(std::log2(size)));
	int32_t level = std::log2(blockSize);

	for (int32_t i = level; i < MAX_LEVELS; ++i)
	{
		if (freeList[i])
		{
			void* block = freeList[i];
			freeList[i] = *reinterpret_cast<void**>(freeList[i]);

			while (i > level)
			{
				--i;
				void* buddy = static_cast<char*>(block) + (static_cast<int32_t>(1) << i);
				*reinterpret_cast<void**>(buddy) = freeList[i];
				freeList[i] = buddy;
			}

			return block;
		}
	}

	return nullptr;
}

void BuddyAllocator::Deallocate(void* ptr, uint64_t size)
{
	uint64_t blockSize = static_cast<uint64_t>(1) << static_cast<uint64_t>(std::ceil(std::log2(size)));
	int32_t level = std::log2(blockSize);

	while (level < MAX_LEVELS - 1)
	{
		void* buddy = GetBuddy(ptr, size);

		if (IsFree(buddy, level))
		{
			RemoveFromFreeList(ptr, level);
			ptr = std::min(ptr, buddy);
			blockSize *= 2;
			++level;
		}
		else
			break;
	}

	*reinterpret_cast<void**>(ptr) = freeList[level];
	freeList[level] = ptr;
}

void* BuddyAllocator::GetBuddy(void* ptr, uint64_t size)
{
	return reinterpret_cast<void*>(reinterpret_cast<uint64_t>(ptr) ^ size);
}

void BuddyAllocator::RemoveFromFreeList(void* ptr, int32_t level)
{
	void** current = &freeList[level];

	while (*current)
	{
		if (*current == ptr)
		{
			*current = *reinterpret_cast<void**>(ptr);
			break;
		}

		current = reinterpret_cast<void**>(*current);
	}
}

bool BuddyAllocator::IsFree(void* buddy, int32_t level) const
{
	void* current = freeList[level];

	while (current)
	{
		if (current == buddy)
		{
			return true;
		}

		current = *reinterpret_cast<void**>(current);
	}

	return false;
}
