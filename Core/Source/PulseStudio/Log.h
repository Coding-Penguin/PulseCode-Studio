#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace PulseStudio {

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
#define PS_CORE_DEBUG(...)		::PulseStudio::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define PS_CORE_TRACE(...)		::PulseStudio::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define PS_CORE_INFO(...)		::PulseStudio::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PS_CORE_WARN(...)		::PulseStudio::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PS_CORE_ERROR(...)		::PulseStudio::Log::GetCoreLogger()->error(__VA_ARGS__)
#define PS_CORE_FATAL(...)		::PulseStudio::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client Log Macros
#define PS_DEBUG(...)		::PulseStudio::Log::GetClientLogger()->debug(__VA_ARGS__)
#define PS_TRACE(...)		::PulseStudio::Log::GetClientLogger()->trace(__VA_ARGS__)
#define PS_INFO(...)		::PulseStudio::Log::GetClientLogger()->info(__VA_ARGS__)
#define PS_WARN(...)		::PulseStudio::Log::GetClientLogger()->warn(__VA_ARGS__)
#define PS_ERROR(...)		::PulseStudio::Log::GetClientLogger()->error(__VA_ARGS__)
#define PS_FATAL(...)		::PulseStudio::Log::GetClientLogger()->critical(__VA_ARGS__)