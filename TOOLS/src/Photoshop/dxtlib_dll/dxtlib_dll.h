#pragma once

#ifdef DXTLIB_EXPORTS
#define DXTLIB_API __declspec(dllexport)
#else
#define DXTLIB_API __declspec(dllimport)
#endif

// Export symbols declared in the static library.
#pragma warning(push)
#	pragma warning( disable : 4267 )
#	include <dxtlib/dxtlib.h>
#pragma warning(pop)


typedef void * (* MallocCallback)(size_t size);
typedef void (* FreeCallback)(void * ptr);

namespace nvDDS
{
	
	DXTLIB_API void registerAllocator(MallocCallback mallocCallback, FreeCallback freeCallback);

} // nvDDS namespace

