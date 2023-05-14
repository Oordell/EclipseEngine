#pragma once

#include "core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace eclipse {

class ECLIPSE_API Log
{
public:
	static void init();

	inline static std::shared_ptr<spdlog::logger>& get_core_logger() { return core_logger_; }
	inline static std::shared_ptr<spdlog::logger>& get_client_logger() { return client_logger_; }

private:
	static std::shared_ptr<spdlog::logger> core_logger_;
	static std::shared_ptr<spdlog::logger> client_logger_;
};

}

// Core logger macros
#define EC_CORE_TRACE(...)   ::eclipse::Log::get_core_logger()->trace(__VA_ARGS__)
#define EC_CORE_DEBUG(...)   ::eclipse::Log::get_core_logger()->debug(__VA_ARGS__)
#define EC_CORE_INFO(...)    ::eclipse::Log::get_core_logger()->info(__VA_ARGS__)
#define EC_CORE_WARN(...)    ::eclipse::Log::get_core_logger()->warn(__VA_ARGS__)
#define EC_CORE_ERROR(...)   ::eclipse::Log::get_core_logger()->error(__VA_ARGS__)
#define EC_CORE_FATAL(...)   ::eclipse::Log::get_core_logger()->critical(__VA_ARGS__)

// Application logger macros
#define EC_TRACE(...)        ::eclipse::Log::get_client_logger()->trace(__VA_ARGS__)
#define EC_DEBUG(...)        ::eclipse::Log::get_client_logger()->debug(__VA_ARGS__)
#define EC_INFO(...)         ::eclipse::Log::get_client_logger()->info(__VA_ARGS__)
#define EC_WARN(...)         ::eclipse::Log::get_client_logger()->warn(__VA_ARGS__)
#define EC_ERROR(...)        ::eclipse::Log::get_client_logger()->error(__VA_ARGS__)
#define EC_FATAL(...)        ::eclipse::Log::get_client_logger()->critical(__VA_ARGS__)