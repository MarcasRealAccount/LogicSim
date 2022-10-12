#include "Log.h"
#include "Core.h"

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Log
{
	static auto& GetSinks()
	{
		static auto s_Sinks = new std::vector<spdlog::sink_ptr>();

		if (!s_Sinks->empty())
			return *s_Sinks;

		if constexpr (Core::s_IsConfigDebug)
			s_Sinks->push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		s_Sinks->push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>("Logs/Log", 23, 59));
		return *s_Sinks;
	}

	std::shared_ptr<spdlog::logger> CreateLogger(const std::string& name)
	{
		auto&                           sinks  = GetSinks();
		std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>(name.substr(0, std::min<std::uint64_t>(name.size(), 16ULL)), sinks.begin(), sinks.end());
		logger->set_pattern("[%T.%f][%16n][%^%8l%$][%7t] %v");

		if constexpr (Core::s_IsConfigDist)
			logger->set_level(spdlog::level::level_enum::trace);
		else
			logger->set_level(spdlog::level::level_enum::trace);

		spdlog::register_logger(logger);
		return logger;
	}

	std::shared_ptr<spdlog::logger> GetMainLogger()
	{
		static auto s_MainLogger = CreateLogger("LogicSim");
		return s_MainLogger;
	}

	std::shared_ptr<spdlog::logger> GetLogger(const std::string& name)
	{
		return spdlog::get(name);
	}

	std::shared_ptr<spdlog::logger> GetOrCreateLogger(const std::string& name)
	{
		auto logger = GetLogger(name);
		if (!logger)
			logger = CreateLogger(name);
		return logger;
	}
} // namespace Log