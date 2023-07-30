#include "dxtlib_dll.h"

namespace 
{
	static MallocCallback s_malloc = malloc;
	static FreeCallback s_free = free;
}


void nvDDS::registerAllocator(MallocCallback mallocCallback, FreeCallback freeCallback)
{
	s_malloc = mallocCallback;
	s_free = freeCallback;
}



// Override global new/delete operators:

void* operator new(size_t size) throw (std::bad_alloc)
{
	void * result = s_malloc(size);
	if (result == NULL) throw std::bad_alloc();
	return result;
}

void* operator new(size_t size, const std::nothrow_t&) throw()
{
	try {
		return ::operator new (size);
	} 
	catch (std::bad_alloc &) {
		return NULL;
	}
}

void* operator new[](size_t size) throw (std::bad_alloc)
{
	void * result = s_malloc(size);
	if (result == NULL) throw std::bad_alloc();
	return result;
}

void* operator new[](size_t size, const std::nothrow_t&) throw()
{
	try {
		return ::operator new[](size);
	} 
	catch (std::bad_alloc &) {
		return NULL;
	}
}

void operator delete(void * ptr) throw()
{
	s_free(ptr);
}

void operator delete(void* ptr, const std::nothrow_t&) throw()
{
	::operator delete(ptr);
}

void operator delete[](void * ptr) throw()
{
	s_free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&) throw()
{
	::operator delete[](ptr);
}




/*
struct MemoryTracker
{
    void * ptr;
    size_t size;
    int num;
};

#define MAX_ALLOCATIONS 50000
static MemoryTracker s_allocations[MAX_ALLOCATIONS];
static int s_allocNum = 0;
static size_t s_totalMemAlloced;

static void * mallocTrack(size_t size)
{
	void * ptr = malloc(size);
	if (ptr == NULL) {
		__debugbreak();
	}

    s_totalMemAlloced += size;

    for (int i = 0; i < MAX_ALLOCATIONS; i++)
    {
        if (s_allocations[i].ptr == NULL)
        {
            s_allocations[i].ptr = ptr;
            s_allocations[i].size = size;
            s_allocations[i].num = s_allocNum++;
            return ptr;
        }
    }

	// Run out of slots!
	__debugbreak();

	return NULL;
}

static bool freeTrack(void* ptr)
{
    for (int i = 0; i < MAX_ALLOCATIONS; i++)
    {
        if (s_allocations[i].ptr == ptr)
        {
			free(ptr);

            s_totalMemAlloced -= s_allocations[i].size;

            s_allocations[i].ptr = NULL;
            s_allocations[i].size = 0;
            s_allocations[i].num = 0;
            return true;
        }
    }

	__debugbreak();

	return false;
}

int freeMemoryleaks()
{
    int total_mem_leaked = 0;

    for (int i = 0; i < MAX_ALLOCATIONS; i++)
    {
        if (s_allocations[i].ptr != NULL)
        {
            s_totalMemAlloced -= s_allocations[i].size;
            total_mem_leaked += s_allocations[i].size;

            printf(" %d bytes at 0x%08X leaked (alloc num: %d)\n", s_allocations[i].size, s_allocations[i].ptr, s_allocations[i].num);

            free(s_allocations[i].ptr);

            s_allocations[i].ptr = NULL;
            s_allocations[i].size = 0;
        }
    }

    return total_mem_leaked;
}
*/

