#pragma once

#include<memory>

#ifdef ET_CORE_ASSERTS
	#define ET_ASSERT(x, ...) { if (!(x)) { ET_ERROR("Assertion Failed: {0}", __VA_ARGS__); /*__debugbreak();*/ } }
	#define ET_CORE_ASSERT(x, ...) { if (!(x)) { ET_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); /*__debugbreak();*/ } }
#else
	#define ET_ASSERT(x, ...)
	#define ET_CORE_ASSERT(x, ...)
#endif

#define ET_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define SCREEN_NULL  -1
#define SCREEN_TOP    0
#define SCREEN_BOTTOM 1

namespace Entry {
	template<typename T>
	using Scope = std::unique_ptr<T>;
	//template<typename T, typename ... Args>
	//constexpr Scope<T> CreateScope(Args&& ... args)
	//{
	//	return std::make_unique<T>(std::forward<Args>(args)...);
	//} // only available in more recent versions of C++ ?

	template<typename T>
	using Ref = std::shared_ptr<T>;
	//template<typename T, typename ... Args>
	//constexpr Scope<T> CreateRef(Args&& ... args)
	//{
	//	return std::make_unique<T>(std::forward<Args>(args)...);
	//}

}