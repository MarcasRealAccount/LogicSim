#pragma once

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>

#include <memory>

namespace Log
{
	std::shared_ptr<spdlog::logger> CreateLogger(const std::string& name);
	std::shared_ptr<spdlog::logger> GetMainLogger();
	std::shared_ptr<spdlog::logger> GetLogger(const std::string& name);
	std::shared_ptr<spdlog::logger> GetOrCreateLogger(const std::string& name);

	template <class... Args>
	void Trace(Args&&... args)
	{
		GetMainLogger()->trace(std::forward<Args>(args)...);
	}

	template <class... Args>
	void Info(Args&&... args)
	{
		GetMainLogger()->info(std::forward<Args>(args)...);
	}

	template <class... Args>
	void Debug(Args&&... args)
	{
		GetMainLogger()->debug(std::forward<Args>(args)...);
	}

	template <class... Args>
	void Warn(Args&&... args)
	{
		GetMainLogger()->warn(std::forward<Args>(args)...);
	}

	template <class... Args>
	void Error(Args&&... args)
	{
		GetMainLogger()->error(std::forward<Args>(args)...);
	}

	template <class... Args>
	void Critical(Args&&... args)
	{
		GetMainLogger()->critical(std::forward<Args>(args)...);
	}
} // namespace Log

//------------
// Formatters
//------------

template <>
struct fmt::formatter<glm::fvec2>
{
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
	{
		auto it = ctx.begin();
		return it;
	}

	template <class FormatContext>
	auto format(const glm::fvec2& vec, FormatContext& ctx) -> decltype(ctx.out())
	{
		return fmt::format_to(ctx.out(), "{{{}, {}}}", vec.x, vec.y);
	}
};

template <>
struct fmt::formatter<glm::fvec3>
{
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
	{
		auto it = ctx.begin();
		return it;
	}

	template <class FormatContext>
	auto format(const glm::fvec3& vec, FormatContext& ctx) -> decltype(ctx.out())
	{
		return fmt::format_to(ctx.out(), "{{{}, {}, {}}}", vec.x, vec.y, vec.z);
	}
};

template <>
struct fmt::formatter<glm::fvec4>
{
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
	{
		auto it = ctx.begin();
		return it;
	}

	template <class FormatContext>
	auto format(const glm::fvec4& vec, FormatContext& ctx) -> decltype(ctx.out())
	{
		return fmt::format_to(ctx.out(), "{{{}, {}, {}, {}}}", vec.x, vec.y, vec.z, vec.w);
	}
};