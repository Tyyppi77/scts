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
	template <typename T, typename = void> struct is_builtin_type : std::false_type { };

	template <typename T>
	inline constexpr bool is_builtin_type_v = is_builtin_type<T>::value;
	template <typename T>
	inline constexpr bool is_serializable_v = is_builtin_type_v<T> || is_registered_type_v<T>;

	// Basic data types.
	// TODO: All string-likes.
	template <> struct is_builtin_type<std::string> : std::true_type { };
	// Note that bool is an arithmetic type, but will most likely require special handling in the formatters.
	template <typename T> struct is_builtin_type<T, typename std::enable_if_t<std::is_arithmetic_v<T>>> : std::true_type { };

	// Enums.
	template <typename E> 
	struct is_builtin_type<E, typename std::enable_if_t<std::is_enum_v<E>>> : std::true_type { };

	// C-style pointers and arrays.
	template <typename T> 
	struct is_builtin_type<T*, typename std::enable_if_t<is_serializable_v<T>>> : std::true_type { };
	template <typename T, std::size_t C>
	struct is_builtin_type<T[C], typename std::enable_if_t<is_serializable_v<T>>> : std::true_type { };

	// Standard library containers and classes.
	template <typename T>
	struct is_builtin_type<std::vector<T>, typename std::enable_if_t<is_serializable_v<T>>> : std::true_type { };
	template <typename T> 
	struct is_builtin_type<std::optional<T>, typename std::enable_if_t<is_serializable_v<T>>> : std::true_type { };
	template <typename V> 
	struct is_builtin_type<std::map<std::string, V>, typename std::enable_if_t<is_serializable_v<V>>> : std::true_type { };
	template <typename T, std::size_t C> 
	struct is_builtin_type<std::array<T, C>, typename std::enable_if_t<is_serializable_v<T>>> : std::true_type { };

	// Standard library smart pointers.
	template <typename T>
	struct is_builtin_type<std::unique_ptr<T>, typename std::enable_if_t<is_serializable_v<T>>> : std::true_type { };
}