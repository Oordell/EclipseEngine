#pragma once

#include "eclipse/core/config.h"
#include "eclipse/core/platform_detection.h"

#include <memory>

/* clang-format off */
// DLL support
#ifdef EC_PLATFORM_WINDOWS
	#if EC_DYNAMIC_LINK
		#ifdef EC_BUILD_DLL
			#define ECLIPSE_API __declspec(dllexport)
		#else
			#define ECLIPSE_API __declspec(dllimport)
		#endif
	#else
		#define ECLIPSE_API
	#endif
#else
	#error Eclipse only supports Windows!
#endif  // End of DLL support

#ifdef ECLIPSE_DEBUG
	#if defined(EC_PLATFORM_WINDOWS)
		#define EC_DEBUGBREAK() __debugbreak()
	#elif defined(EC_PLATFORM_LINUX)
		#include <signal.h>
		#define EC_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define EC_ENABLE_ASSERTS
#else
	#define EC_DEBUGBREAK()
#endif
/* clang-format on */

#ifdef EC_ENABLE_ASSERTS
#define EC_ASSERT(x, ...)                          \
	{                                                 \
		if (!(x)) {                                      \
			EC_FATAL("Assertion Failed: {0}", __VA_ARGS__); \
			EC_DEBUGBREAK();                                \
		}                                                \
	}
#define EC_CORE_ASSERT(x, ...)                          \
	{                                                      \
		if (!(x)) {                                           \
			EC_CORE_FATAL("Assertion Failed: {0}", __VA_ARGS__); \
			EC_DEBUGBREAK();                                     \
		}                                                     \
	}
#else
#define EC_ASSERT(x, ...)
#define EC_CORE_ASSERT(x, ...)
#endif

template <typename T>
constexpr auto BIT(T x) {
	return (1 << x);
}

#define EC_BIND_EVENT_FN(fn) \
	[this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace eclipse {

template <typename T>
using scope = std::unique_ptr<T>;

template <typename T, typename... Args>
scope<T> make_scope(Args&&... args) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using ref = std::shared_ptr<T>;

template <typename T, typename... Args>
ref<T> make_ref(Args&&... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
}

}  // namespace eclipse