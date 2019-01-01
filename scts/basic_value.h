#pragma once

#include <map>
#include <array>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <optional>
#include <type_traits>

#include "register_type.h"

namespace scts {
	// Basic values are more or less trivivally serializable types.

	template <typename T, typename = void> struct is_basic_value : std::false_type { };

	template <typename T>
	inline constexpr bool is_basic_value_v = is_basic_value<T>::value;
	template <typename T>
	inline constexpr bool is_serializable_v = is_basic_value_v<T> || is_registered_type_v<T>;

	// Basic data types.
	template <> struct is_basic_value<int> : std::true_type { };
	template <> struct is_basic_value<bool> : std::true_type { };
	template <> struct is_basic_value<float> : std::true_type { };
	template <> struct is_basic_value<double> : std::true_type { };
	template <> struct is_basic_value<uint8_t> : std::true_type { };
	template <> struct is_basic_value<std::string> : std::true_type { };

	// Enums.
	template <typename E> 
	struct is_basic_value<E, typename std::enable_if_t<std::is_enum_v<E>>> : std::true_type { };

	// C-style pointers and arrays.
	template <typename T> 
	struct is_basic_value<T*, typename std::enable_if_t<is_serializable_v<T>>> : std::true_type { };
	template <typename T, std::size_t C>
	struct is_basic_value<T[C], typename std::enable_if_t<is_serializable_v<T>>> : std::true_type { };

	// Standard library containers and classes.
	template <typename T>
	struct is_basic_value<std::vector<T>, typename std::enable_if_t<is_serializable_v<T>>> : std::true_type { };
	template <typename T> 
	struct is_basic_value<std::optional<T>, typename std::enable_if_t<is_serializable_v<T>>> : std::true_type { };
	template <typename V> 
	struct is_basic_value<std::map<std::string, V>, typename std::enable_if_t<is_serializable_v<V>>> : std::true_type { };
	template <typename T, std::size_t C> 
	struct is_basic_value<std::array<T, C>, typename std::enable_if_t<is_serializable_v<T>>> : std::true_type { };

	// Standard library smart pointers.
	template <typename T>
	struct is_basic_value<std::unique_ptr<T>, typename std::enable_if_t<is_serializable_v<T>>> : std::true_type { };
}