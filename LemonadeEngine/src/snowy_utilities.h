#pragma once

#ifndef LEMONADE_ENGINE_SRC_SNOWY_UTILITIES_H
#define LEMONADE_ENGINE_SRC_SNOWY_UTILITIES_H

#include <tuple>
#include <memory>
#include <utility>

template <typename... Args>
auto deep_copy_tuple(const std::tuple<std::unique_ptr<Args>...>& original) 
{
	return std::apply(
		[&](const auto&... ptr) 
		{
			return std::make_tuple(ptr ? std::make_unique<Args>(*ptr) : nullptr ...);
		}, 
		original
	);
}

#endif // !LEMONADE_ENGINE_SRC_SNOWY_UTILITIES_H
