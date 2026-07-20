#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace PulseCode {

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Core Log Macros
#define PS_CORE_DEBUG(...)		::PulseCode::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define PS_CORE_TRACE(...)		::PulseCode::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define PS_CORE_INFO(...)		::PulseCode::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PS_CORE_WARN(...)		::PulseCode::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PS_CORE_ERROR(...)		::PulseCode::Log::GetCoreLogger()->error(__VA_ARGS__)
#define PS_CORE_FATAL(...)		::PulseCode::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client Log Macros
#define PS_DEBUG(...)		::PulseCode::Log::GetClientLogger()->debug(__VA_ARGS__)
#define PS_TRACE(...)		::PulseCode::Log::GetClientLogger()->trace(__VA_ARGS__)
#define PS_INFO(...)		::PulseCode::Log::GetClientLogger()->info(__VA_ARGS__)
#define PS_WARN(...)		::PulseCode::Log::GetClientLogger()->warn(__VA_ARGS__)
#define PS_ERROR(...)		::PulseCode::Log::GetClientLogger()->error(__VA_ARGS__)
#define PS_FATAL(...)		::PulseCode::Log::GetClientLogger()->critical(__VA_ARGS__)