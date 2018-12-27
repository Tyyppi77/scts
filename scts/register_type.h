#pragma once

#include <type_traits>

namespace scts {
	template <typename T>
	struct register_type : std::false_type { };

	using allow_serialization = std::true_type;

	template <typename T>
	inline constexpr bool is_registered_type_v = register_type<T>::value;
}