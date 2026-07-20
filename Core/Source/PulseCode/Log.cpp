#include "pspch.h"

#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks-inl.h>

namespace PulseCode {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger = nullptr;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger = nullptr;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] [Thread %t] %n -> %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("PulseCode");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("Application");
		s_ClientLogger->set_level(spdlog::level::trace);
	}

}
