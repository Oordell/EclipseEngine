#pragma once

#include "eclipse/core/core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#include <chrono>

namespace eclipse {

class ECLIPSE_API Log {
public:
	static void init();

	inline static ref<spdlog::logger>& get_core_logger() { return core_logger_; }

	inline static ref<spdlog::logger>& get_client_logger() { return client_logger_; }

private:
	static ref<spdlog::logger> core_logger_;
	static ref<spdlog::logger> client_logger_;
};
}  // namespace eclipse

// Core logger macros
#define EC_CORE_TRACE(...) ::eclipse::Log::get_core_logger()->trace(__VA_ARGS__)
#define EC_CORE_DEBUG(...) ::eclipse::Log::get_core_logger()->debug(__VA_ARGS__)
#define EC_CORE_INFO(...)  ::eclipse::Log::get_core_logger()->info(__VA_ARGS__)
#define EC_CORE_WARN(...)  ::eclipse::Log::get_core_logger()->warn(__VA_ARGS__)
#define EC_CORE_ERROR(...) ::eclipse::Log::get_core_logger()->error(__VA_ARGS__)
#define EC_CORE_FATAL(...) ::eclipse::Log::get_core_logger()->critical(__VA_ARGS__)

#define EC_CORE_TRACE_THROTTLED(rate, ...)                                                                   \
	{                                                                                                           \
		static std::chrono::steady_clock::time_point last_hit;                                                     \
		auto now = std::chrono::steady_clock::now();                                                               \
		if (std::chrono::duration_cast<std::chrono::microseconds>(now - last_hit).count() >= (rate * 1000000.0)) { \
			last_hit = std::chrono::steady_clock::now();                                                              \
			EC_CORE_TRACE(__VA_ARGS__);                                                                               \
		}                                                                                                          \
	}

#define EC_CORE_DEBUG_THROTTLED(rate, ...)                                                                   \
	{                                                                                                           \
		static std::chrono::steady_clock::time_point last_hit;                                                     \
		auto now = std::chrono::steady_clock::now();                                                               \
		if (std::chrono::duration_cast<std::chrono::microseconds>(now - last_hit).count() >= (rate * 1000000.0)) { \
			last_hit = std::chrono::steady_clock::now();                                                              \
			EC_CORE_DEBUG(__VA_ARGS__);                                                                               \
		}                                                                                                          \
	}

#define EC_CORE_INFO_THROTTLED(rate, ...)                                                                    \
	{                                                                                                           \
		static std::chrono::steady_clock::time_point last_hit;                                                     \
		auto now = std::chrono::steady_clock::now();                                                               \
		if (std::chrono::duration_cast<std::chrono::microseconds>(now - last_hit).count() >= (rate * 1000000.0)) { \
			last_hit = std::chrono::steady_clock::now();                                                              \
			EC_CORE_INFO(__VA_ARGS__);                                                                                \
		}                                                                                                          \
	}

#define EC_CORE_WARN_THROTTLED(rate, ...)                                                                    \
	{                                                                                                           \
		static std::chrono::steady_clock::time_point last_hit;                                                     \
		auto now = std::chrono::steady_clock::now();                                                               \
		if (std::chrono::duration_cast<std::chrono::microseconds>(now - last_hit).count() >= (rate * 1000000.0)) { \
			last_hit = std::chrono::steady_clock::now();                                                              \
			EC_CORE_WARN(__VA_ARGS__);                                                                                \
		}                                                                                                          \
	}

#define EC_CORE_ERROR_THROTTLED(rate, ...)                                                                   \
	{                                                                                                           \
		static std::chrono::steady_clock::time_point last_hit;                                                     \
		auto now = std::chrono::steady_clock::now();                                                               \
		if (std::chrono::duration_cast<std::chrono::microseconds>(now - last_hit).count() >= (rate * 1000000.0)) { \
			last_hit = std::chrono::steady_clock::now();                                                              \
			EC_CORE_ERROR(__VA_ARGS__);                                                                               \
		}                                                                                                          \
	}

#define EC_CORE_FATAL_THROTTLED(rate, ...)                                                                   \
	{                                                                                                           \
		static std::chrono::steady_clock::time_point last_hit;                                                     \
		auto now = std::chrono::steady_clock::now();                                                               \
		if (std::chrono::duration_cast<std::chrono::microseconds>(now - last_hit).count() >= (rate * 1000000.0)) { \
			last_hit = std::chrono::steady_clock::now();                                                              \
			EC_CORE_FATAL(__VA_ARGS__);                                                                               \
		}                                                                                                          \
	}

// Application logger macros
#define EC_TRACE(...) ::eclipse::Log::get_client_logger()->trace(__VA_ARGS__)
#define EC_DEBUG(...) ::eclipse::Log::get_client_logger()->debug(__VA_ARGS__)
#define EC_INFO(...)  ::eclipse::Log::get_client_logger()->info(__VA_ARGS__)
#define EC_WARN(...)  ::eclipse::Log::get_client_logger()->warn(__VA_ARGS__)
#define EC_ERROR(...) ::eclipse::Log::get_client_logger()->error(__VA_ARGS__)
#define EC_FATAL(...) ::eclipse::Log::get_client_logger()->critical(__VA_ARGS__)

#define EC_TRACE_THROTTLED(rate, ...)                                                                        \
	{                                                                                                           \
		static std::chrono::steady_clock::time_point last_hit;                                                     \
		auto now = std::chrono::steady_clock::now();                                                               \
		if (std::chrono::duration_cast<std::chrono::microseconds>(now - last_hit).count() >= (rate * 1000000.0)) { \
			last_hit = std::chrono::steady_clock::now();                                                              \
			EC_TRACE(__VA_ARGS__);                                                                                    \
		}                                                                                                          \
	}

#define EC_DEBUG_THROTTLED(rate, ...)                                                                        \
	{                                                                                                           \
		static std::chrono::steady_clock::time_point last_hit;                                                     \
		auto now = std::chrono::steady_clock::now();                                                               \
		if (std::chrono::duration_cast<std::chrono::microseconds>(now - last_hit).count() >= (rate * 1000000.0)) { \
			last_hit = std::chrono::steady_clock::now();                                                              \
			EC_DEBUG(__VA_ARGS__);                                                                                    \
		}                                                                                                          \
	}

#define EC_INFO_THROTTLED(rate, ...)                                                                         \
	{                                                                                                           \
		static std::chrono::steady_clock::time_point last_hit;                                                     \
		auto now = std::chrono::steady_clock::now();                                                               \
		if (std::chrono::duration_cast<std::chrono::microseconds>(now - last_hit).count() >= (rate * 1000000.0)) { \
			last_hit = std::chrono::steady_clock::now();                                                              \
			EC_INFO(__VA_ARGS__);                                                                                     \
		}                                                                                                          \
	}

#define EC_WARN_THROTTLED(rate, ...)                                                                         \
	{                                                                                                           \
		static std::chrono::steady_clock::time_point last_hit;                                                     \
		auto now = std::chrono::steady_clock::now();                                                               \
		if (std::chrono::duration_cast<std::chrono::microseconds>(now - last_hit).count() >= (rate * 1000000.0)) { \
			last_hit = std::chrono::steady_clock::now();                                                              \
			EC_WARN(__VA_ARGS__);                                                                                     \
		}                                                                                                          \
	}

#define EC_ERROR_THROTTLED(rate, ...)                                                                        \
	{                                                                                                           \
		static std::chrono::steady_clock::time_point last_hit;                                                     \
		auto now = std::chrono::steady_clock::now();                                                               \
		if (std::chrono::duration_cast<std::chrono::microseconds>(now - last_hit).count() >= (rate * 1000000.0)) { \
			last_hit = std::chrono::steady_clock::now();                                                              \
			EC_ERROR(__VA_ARGS__);                                                                                    \
		}                                                                                                          \
	}

#define EC_FATAL_THROTTLED(rate, ...)                                                                        \
	{                                                                                                           \
		static std::chrono::steady_clock::time_point last_hit;                                                     \
		auto now = std::chrono::steady_clock::now();                                                               \
		if (std::chrono::duration_cast<std::chrono::microseconds>(now - last_hit).count() >= (rate * 1000000.0)) { \
			last_hit = std::chrono::steady_clock::now();                                                              \
			EC_FATAL(__VA_ARGS__);                                                                                    \
		}                                                                                                          \
	}
