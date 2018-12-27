#pragma once

namespace scts {
	template <typename T>
	struct deduce_member_ptr_type {
		using type = T;
	};

	template <typename Parent, typename T>
	struct deduce_member_ptr_type<T Parent::*> {
		using type = T;
	};
}