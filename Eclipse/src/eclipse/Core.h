#pragma once

#ifdef EC_PLATFORM_WINDOWS
	#ifdef EC_BUILD_DLL
		#define ECLIPSE_API _declspec(dllexport)
	#else
		#define ECLIPSE_API _declspec(dllimport)
	#endif
#else
	#error Eclipse only supports Windows!
#endif

#ifdef EC_ENABLE_ASSERTS
	#define EC_ASSERT(x, ...) { if(!(x)) { EC_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define EC_CORE_ASSERT(x, ...) { if (!(x)) { EC_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define EC_ASSERT(x, ...) 
	#define EC_CORE_ASSERT(x, ...)
#endif

template<typename T>
constexpr auto BIT(T x) { return (1 << x); }

#define EC_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)