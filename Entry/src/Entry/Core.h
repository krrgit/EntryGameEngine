#pragma once

#ifdef ET_CORE_ASSERTS
	#define ET_ASSERT(x, ...) { if (!(x)) { ET_ERROR("Assertion Failed: {0}", __VA_ARGS__); /*__debugbreak();*/ } }
	#define ET_CORE_ASSERT(x, ...) { if (!(x)) { ET_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); /*__debugbreak();*/ } }
#else
	#define ET_ASSERT(x, ...)
	#define ET_CORE_ASSERT(x, ...)
#endif

#define SCREEN_NULL  -1
#define SCREEN_TOP    0
#define SCREEN_BOTTOM 1


#define ET_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)