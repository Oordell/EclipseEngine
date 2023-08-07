#pragma once

#include "eclipse/core/config.h"

#include <memory>

/* clang-format off */
// Platform detection using predefined macros
#ifdef _WIN32
	/* Windows x64/x86 */
	#ifdef _WIN64
		/* Windows x64  */
		#ifndef EC_PLATFORM_WINDOWS
			#define EC_PLATFORM_WINDOWS
		#endif
	#else
		/* Windows x86 */
		#error "x86 Builds are not supported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	/* TARGET_OS_MAC exists on all the platforms so we must check all of them (in this order) 
	 * to ensure that we're running on and not some other Apple platform 
	 */
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		#define EC_PLATFORM_IOS
		#error "IOS is not supported!"
	#elif TARGET_OS_MAC == 1
		#define EC_PLATFORM_MACOS
		#error "MacOS is not supported!"
	#else
		#error "Unknown Apple platform!"
	#endif
	/* We also have to check __ANDROID__ before __linux__ since android is based on the linux 
	 * kernel it has __linux__ defined 
	 */
#elif defined(__ANDROID__)
	#define EC_PLATFORM_ANDROID
	#error "Android is not supported!"
#elif defined(__linux__)
	#define EC_PLATFORM_LINUX
	#error "Linux is not supported!"
#else
	/* Unknown compiler/platform */
	#error "Unknown platform!"
#endif  // End of platform detection

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
			EC_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
			EC_DEBUGBREAK();                                \
		}                                                \
	}
#define EC_CORE_ASSERT(x, ...)                          \
	{                                                      \
		if (!(x)) {                                           \
			EC_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
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