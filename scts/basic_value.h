#pragma once

#include <map>
#include <array>
#include <string>
#include <vector>
#include <type_traits>

namespace scts {
	template <typename T> struct is_basic_value : std::false_type { };
	template <> struct is_basic_value<int> : std::true_type { };
	template <> struct is_basic_value<bool> : std::true_type { };
	template <> struct is_basic_value<float> : std::true_type { };
	template <> struct is_basic_value<double> : std::true_type { };
	template <> struct is_basic_value<std::string> : std::true_type { };
	template <typename T> struct is_basic_value<std::vector<T>> : std::true_type { };
	template <typename T, std::size_t C> struct is_basic_value<std::array<T, C>> : std::true_type { };
	template <typename V> struct is_basic_value<std::map<std::string, V>> : std::true_type { };

	template <typename T>
	inline constexpr bool is_basic_value_v = is_basic_value<T>::value;
}