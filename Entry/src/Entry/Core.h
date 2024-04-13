#pragma once


#ifdef ET_CORE_ASSERTS
	#define ET_ASSERT(x, ...) { if (!(x)) { HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); /*__debugbreak();*/ } }
	#define ET_CORE_ASSERT(x, ...) { if (!(x)) { HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); /*__debugbreak();*/ } }
#else
	#define ET_ASSERT(x, ...)
	#define ET_CORE_ASSERT(x, ...)
#endif

