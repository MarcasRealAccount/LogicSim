#pragma once

#include <concepts>

template <class F, class R, class... Args>
concept Callable = requires(F function, Args&&... args)
{
	{
		function(std::forward<Args>(args)...)
		} -> std::convertible_to<R>;
};